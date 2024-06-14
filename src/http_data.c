/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/http_data.c
 * @brief implementation of http_data.h
*/

#include "http_data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *public_path = NULL;

/**
 * Initialize a request_t struct
 * This function allocates memory for a new request_t struct and initializes its fields to NULL.
 * 
 * @return a pointer to the request_t struct
 * @return NULL if an error occurred
*/
request_t *init_request()
{
    request_t *request = (request_t *)malloc(sizeof(request_t));
    if (request == NULL)
    {
        perror("[-]Error malloc");
        return NULL;
    }
    request->method = NULL;
    request->path = NULL;
    request->version = NULL;
    request->headers = NULL;
    request->body.data = NULL;
    request->body.params = NULL;
    request->body.n_params = 0;
    return request;
}

/**
 * Initialize a response_t struct
 * This function allocates memory for a new response_t struct and initializes its fields to NULL.
 * 
 * @return a pointer to the response_t struct
 * @return NULL if an error occurred
*/
response_t *init_response()
{
    response_t *response = (response_t *)malloc(sizeof(request_t));
    if (response == NULL)
    {
        printf("[-]Error malloc");
        return NULL;
    }
    response->version = NULL;
    response->status_code = NULL;
    response->headers = NULL;
    response->body = NULL;
    return response;
}

/**
 * Free a request_t struct
 * This function frees the memory allocated for a request_t struct.
 * 
 * @param req a pointer to the request_t struct
*/
void free_request(request_t *req)
{
    if (req != NULL)
    {
        free(req->method);
        free(req->path);
        free(req->version);
        free_list(req->headers);
        free(req->body.data);
        free_list(req->body.params);
        free(req); 
        req = NULL;
    }
}

/**
 * Free a response_t struct
 * This function frees the memory allocated for a response_t struct.
 * 
 * @param res a pointer to the response_t struct
*/
void free_response(response_t *res)
{
    if (res != NULL)
    {
        free(res->version);
        free(res->status_code);
        free_list(res->headers);
        free(res->body);
        free(res);
        res = NULL;
    }
}

/**
 * Get a header value
 * This function searches for a header in a list of headers and returns its value.
 * 
 * @param headers a pointer to the list of headers
 * @param key the key of the header to search for
 * 
 * @return the value of the header
 * @return NULL if the header is not found
*/
char *get_header(node_t *headers, char *key)
{
    node_t *node = search_node(headers, key);
    if (node == NULL)
        return NULL;
    return node->value;
}

/**
 * Add a header
 * This function adds a header to a list of headers.
 * 
 * @param headers a pointer to the list of headers
 * @param key the key of the header
 * @param value the value of the header
*/
void add_header(node_t **headers, char *key, char *value)
{
    insert_node(headers, key, value);
}

/**
 * Add a parameter
 * This function adds a parameter to a list of parameters.
 * 
 * @param params a pointer to the list of parameters
 * @param key the key of the parameter
 * @param value the value of the parameter
*/
void add_param(node_t **params, char *key, char *value)
{
    insert_node(params, key, value);
}

void add_body_res(response_t *res, char *body)
{
    char *body_copy = (char *)malloc(strlen(body) + 1);
    if (body_copy == NULL)
    {
        perror("[-]Error malloc");
        return;
    }
    strcpy(body_copy, body);
    res->body = body_copy;
}

void set_public_path(char *path)
{
    public_path = path;
}

//start path is public/static
void add_file_body(response_t *res, char *file_name)
{
    char path [100];
    sprintf(path, "%s/static/%s", public_path, file_name);
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        perror("[-]Error file is not found");
        return;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *body = (char *)malloc(size + 1);
    if (body == NULL)
    {
        perror("[-]Error malloc");
        return;
    }
    fread(body, 1, size, file);
    fclose(file);
    body[size] = '\0';
    add_body_res(res, body);
    free(body);
}

void add_version_res(response_t *res, char *version)
{
    char *version_copy = (char *)malloc(strlen(version) + 1);
    if (version_copy == NULL)
    {
        perror("[-]Error malloc");
        return;
    }
    strcpy(version_copy, version);
    res->version = version_copy;
}

void add_status_code_res(response_t *res, char *status_code)
{
    char *status_code_copy = (char *)malloc(strlen(status_code) + 1);
    if (status_code_copy == NULL)
    {
        perror("[-]Error malloc");
        return;
    }
    strcpy(status_code_copy, status_code);
    res->status_code = status_code_copy;
}

/**
 * Print a request
 * This function prints the fields of a request_t struct.
 * 
 * @param req a pointer to the request_t struct
*/
void print_request (request_t *req)
{
    if (req == NULL)
    {
        perror("[-]Invalid data");
        return;
    }
    printf("Method: %s\n", req->method);
    printf("Path: %s\n", req->path);
    printf("Version: HTTP/%s\n", req->version);
    printf("Headers:\n");
    print_list(req->headers);
    printf("Body:");
    printf("n_params: %ld\n", req->body.n_params);
    printf("Params:\n");
    print_list(req->body.params);
    printf("Data: %s\n", req->body.data);
}

/**
 * Print a response
 * This function prints the fields of a response_t struct.
 * 
 * @param res a pointer to the response_t struct
*/
void print_response (response_t *res)
{
    if (res == NULL)
    {
        perror("[-]Invalid data");
        return;
    }
    printf("Version: HTTP/%s\n", res->version);
    printf("Status code: %s\n", res->status_code);
    printf("Headers:\n");
    print_list(res->headers);
    printf("Body: %s\n", res->body);
}
