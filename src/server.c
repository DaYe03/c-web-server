/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/server.c
 * @brief implementation of server.h
*/

#include "server.h"
#include "http_data.h"
#include "process_request.h"
#include "process_response.h"
#include "route.h"
#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 2000
#define MAX_SIZE 1048576

enum
{
    STATE_FIRST_LINE = 0,
    STATE_HEADERS = 1,
    STATE_BODY = 2,
    STATE_ELABORATE_RESPONSE = 3,
    STATE_RESET = 4
};

/**
 * Serialize a response_t struct to a string
 *  
 * @param res response_t struct
 * @return serialized response or NULL if an error occurred
*/
char *serialize(response_t *res)
{
    if (validate_response(res) < 0)
    {
        return NULL;
    }

    char *serialized = malloc(MAX_SIZE * sizeof(char));
    if (serialized == NULL)
    {
        perror("[-]malloc failed");
        return NULL;
    }
    serialized[0] = '\0';

    char *status_message;
    if ((status_message = get_status_message(res)) == "Unknown")
    {
        perror("[-]Invalid status code");
        return NULL;
    }

    sprintf(serialized, "HTTP/%s %s %s\r\n", res->version, res->status_code, status_message);
    char *headers = headers_to_string(res);
    strcat(serialized, headers);
    strcat(serialized, "\r\n");

    if (res->body != NULL)
        strcat(serialized, res->body);

    free(headers);
    return serialized;
}

/**
 * Send a response to a client
 * 
 * @param client_fd client file descriptor
 * @param res response_t struct
 * @return 0 if the response was sent successfully, -1 otherwise
*/
int send_response(client_t *client)
{
    char *response = serialize(client->res);
    if (response == NULL)
        return -1;

    if (send(client->client_fd, response, strlen(response), 0) < 0)
    {
        perror("[-]send failed");
        return -1;
    }
    free(response);
    return 0;
}

/**
 * Send an error response to a client
 * 
 * @param client_fd client file descriptor
 * @param status_code status code
 * @param message error message
 * @return 0 if the error response was sent successfully, -1 otherwise
*/
int send_error(client_t *client, char *status_code, char *message)
{
    response_t *res = init_response();
    free_response(client->res);
    client->res = res;
    add_status_code_res(res, status_code);
    add_body_res(res, message);
    add_header(&(res->headers), "Content-Type", "text/plain");
    send_response(client);
    return 0;
}

/**
 * Parse the first line of a request
 * 
 * @param req request_t struct
 * @param line first line of the request
 * @return 0 if the first line was parsed successfully, -1 otherwise
*/
int handle_line(client_t *client, char *request)
{
    char *end_of_line = strstr(request, "\r\n");
    char *line = strndup(request, end_of_line - request + 2);
    if (line == NULL)
    {
        perror("[-]strndup failed");
        send_error(client, "500", "Internal Server Error");
        return -1;
    }
    else
    {
        if (parse_first_line(client->req, line) < 0)
        {
            send_error(client, "400", "Bad Request");
            free(line);
            return -1;
        }
        memmove(request, end_of_line + 2, strlen(end_of_line + 2) + 1);
        free(line);
    }
    return 0;
}

/**
 * Parse the headers of a request
 * 
 * @param req request_t struct
 * @param headers headers of the request
 * @return 0 if the headers were parsed successfully, -1 otherwise
*/
int handle_headers(client_t *client, char *request)
{
    char *end_of_headers = strstr(request, "\r\n\r\n");
    char *headers = strndup(request, end_of_headers - request + 4);
    if (headers == NULL)
    {
        perror("[-]strndup failed");
        send_error(client, "500", "Internal Server Error");
        return -1;
    }
    if (parse_headers(client->req, headers) < 0)
    {
        send_error(client, "400", "Bad Request");
        free(headers);
        return -1;
    }
    free(headers);
    return 0;
}

/**
 * Parse the body of a request
 * 
 * @param req request_t struct
 * @param request request string
 * @return 0 if the body was parsed successfully, -1 otherwise
*/
int handle_body(client_t *client, char *request)
{
    size_t content_length = strtoul(get_header(client->req->headers, "Content-Length"), NULL, 10);
    if (content_length < 0)
    {
        send_error(client, "400", "Bad Request");
        return -1;
    }
    if (content_length > MAX_SIZE)
    {
        send_error(client, "413", "Request Entity Too Large");
        return -1;
    }
    if (parse_body(client->req, request, content_length) < 0)
    {
        send_error(client, "400", "Bad Request");
        return -1;
    }
    return 0;
}

/**
 * Handle a response
 * 
 * @param client_fd client file descriptor
 * @param req request_t struct
 * @param res response_t struct
 * @return 0 if the response was handled successfully, -1 otherwise
*/
int handle_response(client_t *client)
{
    char *path = client->req->path;
    char *method = client->req->method;
    callback cb = get_route(method, path);
    if (cb == NULL)
    {
        if (strcmp(method, "GET") == 0)
        {
            
        }
        send_error(client, "404", "Not Found");
        return -1;
    }
    cb(client->req, client->res);
    return 0;
}

/**
 * Handle a request
 * 
 * @param arg client file descriptor
 * @return NULL
*/
void *handle_request(void *arg)
{
    client_t *client = get_client(*(int *)arg);
    free(arg);
    if (client->client_fd < 0)
    {
        perror("[-]Invalid client file descriptor");
        return NULL;
    }

    int state = STATE_FIRST_LINE;
    size_t received, total_received = 0;
    char *buffer = malloc(BUFFER_SIZE * sizeof(char) + 1);
    char *request = malloc(MAX_SIZE * sizeof(char) + 1);
    if (buffer == NULL || request == NULL)
    {
        perror("[-]malloc failed");
        send_error(client, "500", "Internal Server Error");
    }
    else
    {
        request[0] = '\0';
        while ((received = recv(client->client_fd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            buffer[received] = '\0';
            if ((total_received += received) > MAX_SIZE)
            {
                send_error(client, "413", "Request Entity Too Large");
                state = STATE_RESET;
            }
            else
            {
                memcpy(request + strlen(request), buffer, received);
                request[strlen(request)] = '\0';
            }
            if (state == STATE_FIRST_LINE)
            {
                if (strstr(request, "\r\n") != NULL)
                {
                    if (handle_line(client, request) < 0)
                        state = STATE_RESET;
                    else
                        state = STATE_HEADERS;
                }
            }
            if (state == STATE_HEADERS)
            {
                char * end_of_headers = strstr(request, "\r\n\r\n");
                if (end_of_headers != NULL)
                {
                    if (handle_headers(client, request) < 0)
                        state = STATE_RESET;
                    else
                    {
                        if (get_header(client->req->headers, "Content-Length") != NULL)
                            state = STATE_BODY;
                        else
                            state = STATE_ELABORATE_RESPONSE;
                    }
                    //remove headers from request
                    memmove(request, end_of_headers + 4, strlen(end_of_headers + 4) + 1); 
                    total_received -= strlen(end_of_headers) + 4;
                }
            }
            if (state == STATE_BODY)
            {
                size_t content_length = strtoul(get_header(client->req->headers, "Content-Length"), NULL, 10);
                size_t request_length = strlen(request);
                if (request_length == content_length)
                {
                    if (handle_body(client, request) < 0)
                        state = STATE_RESET;
                    else
                        state = STATE_ELABORATE_RESPONSE;
                }
                else if (request_length > content_length)
                {
                    send_error(client, "400", "Bad Request");
                    state = STATE_RESET;
                }
            }
            if (state == STATE_ELABORATE_RESPONSE)
            {
                if (handle_response(client) < 0)
                    perror("[-]Error handling response");
                else
                    send_response(client);
                state = STATE_RESET;
            }
            if (state == STATE_RESET)
            {
                free_request(client->req);
                free_response(client->res);
                client->req = init_request();
                client->res = init_response();
                request[0] = '\0';
                state = STATE_FIRST_LINE;
            }
        }
    }
    free(request);
    free(buffer);
    remove_client(client->client_fd); //remove client from clients list and close the connection with the client
    printf("[+]Client disconnected\n");
    return NULL;
}

void *run_server(void *arg)
{
    server_t *server = (server_t *)arg;
    init_clients(); //set clients list to NULL
    while (1)
    {
        struct sockaddr_in client_addr;
        int *client_fd = malloc(sizeof(int));
        socklen_t client_addr_len = sizeof(client_addr);
        pthread_t thread_id;

        client_t *client = (client_t *)malloc(sizeof(client_t));
        if (client == NULL)
        {
            perror("[-]malloc failed");
            free(client_fd);
            return NULL;
        }

        if ((*client_fd = accept(server->server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("[-]accept failed");
            free(client_fd);
            free(client);
            return NULL;
        }

        client->client_fd = *client_fd;
        client->req = init_request(); //malloc request and set all fields to NULL
        client->res = init_response(); //malloc response and set all fields to NULL
        client->thread_id = thread_id;
        client->next = NULL;

        if (add_client(client) < 0 || client->req == NULL || client->res == NULL)
        {
            free(client_fd);
            free(client);
            return NULL;
        }

        if (pthread_create(&thread_id, NULL, handle_request, (void *)client_fd) < 0)
        {
            perror("[-]pthread_create failed");
            return NULL;
        }

        if (pthread_detach(thread_id) < 0)
        {
            perror("[-]pthread_detach failed");
            return NULL;
        }
    }
    return NULL;
}

server_t *start_daemon(int port, int max_connections, const char *ip)
{
    server_t *server = (server_t *)malloc(sizeof(server_t));
    if (server == NULL)
    {
        perror("[-]malloc failed");
        return NULL;
    }
    
    server->port = port;
    server->max_connections = max_connections;

    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("[-]socket failed");
        free(server);
        return NULL;
    }

    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("[-]setsockopt failed");
        close(server->server_fd);
        free(server);
        return NULL;
    }

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = ip == NULL ? INADDR_ANY : inet_addr(ip);
    server->server_addr.sin_port = htons(port);

    if (bind(server->server_fd, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) < 0)
    {
        perror("[-]bind failed");
        close(server->server_fd);
        free(server);
        return NULL;
    }

    if (listen(server->server_fd, max_connections) < 0)
    {
        perror("[-]listen failed");
        close(server->server_fd);
        free(server);
        return NULL;
    }

    printf("[+]Server started at port %d\n", port);

    // Create a daemon thread
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, run_server, (void *)server) < 0)
    {
        perror("[-]pthread_create failed");
        close(server->server_fd);
        free(server);
        return NULL;
    }
    server->thread_id = thread_id;

    if (pthread_detach(thread_id) < 0)
    {
        perror("[-]pthread_detach failed");
        close(server->server_fd);
        free(server);
        return NULL;
    }
    return server;
}

void stop_daemon(server_t *server)
{
    free_clients();
    close(server->server_fd);
    pthread_cancel(server->thread_id);
    free(server);
    printf("[+]Server stopped\n");
}