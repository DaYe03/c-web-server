/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/process_response.c
 * @brief implementation of process_response.h
*/

#include "process_response.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Get the status message
 * This function returns the status message corresponding to the status code.
 * 
 * @param res a pointer to the response_t struct
 * @return the status message or "Unknown" if the status code is not recognized
 */
char *get_status_message(response_t *res)
{
    char *status_code = res->status_code;
    if (strcmp(status_code, "100") == 0)
        return "Continue";
    else if (strcmp(status_code, "101") == 0)
        return "Switching Protocols";
    else if (strcmp(status_code, "200") == 0)
        return "OK";
    else if (strcmp(status_code, "201") == 0)
        return "Created";
    else if (strcmp(status_code, "202") == 0)
        return "Accepted";
    else if (strcmp(status_code, "204") == 0)
        return "No Content";
    else if (strcmp(status_code, "301") == 0)
        return "Moved Permanently";
    else if (strcmp(status_code, "302") == 0)
        return "Found";
    else if (strcmp(status_code, "304") == 0)
        return "Not Modified";
    else if (strcmp(status_code, "400") == 0)
        return "Bad Request";
    else if (strcmp(status_code, "401") == 0)
        return "Unauthorized";
    else if (strcmp(status_code, "403") == 0)
        return "Forbidden";
    else if (strcmp(status_code, "404") == 0)
        return "Not Found";
    else if (strcmp(status_code, "413") == 0)
        return "Payload Too Large";
    else if (strcmp(status_code, "415") == 0)
        return "Unsupported Media Type";
    else if (strcmp(status_code, "500") == 0)
        return "Internal Server Error";
    else if (strcmp(status_code, "501") == 0)
        return "Not Implemented";
    else if (strcmp(status_code, "502") == 0)
        return "Bad Gateway";
    else if (strcmp(status_code, "503") == 0)
        return "Service Unavailable";
    return "Unknown";
}

/**
 * Convert the headers to a string
 * This function converts the headers of a response to a string.
 * 
 * @param res a pointer to the response_t struct
 * @return a string containing the headers
 */
char *headers_to_string(response_t *res)
{
    size_t max_size = 4096;
    node_t *current = res->headers;
    char *headers;
    headers = (char *)malloc(max_size);
    if (headers == NULL)
    {
        perror("[-]Error creating headers string");
        return NULL;
    }
    headers[0] = '\0';
    while (current != NULL)
    {
        if (current->key != NULL && current->value != NULL)
        {
            size_t headers_len = strlen(current->key) + strlen(current->value) + 4;
            if (strlen(headers) + headers_len + 1 > max_size)
            {
                max_size *= 2;
                headers = (char *)realloc(headers, max_size);
                if (headers == NULL)
                {
                    perror("[-]Error creating headers string");
                    return NULL;
                }
            }
            strcat(headers, current->key);
            strcat(headers, ": ");
            strcat(headers, current->value);
            strcat(headers, "\r\n");
        }
        current = current->next;
    }
    return headers;
}

/**
 * Get the content length
 * This function returns the content length of the response body.
 * 
 * @param res a pointer to the response_t struct
 * @return the content length or "0" if the body is NULL
 */
char *content_length(response_t *res)
{
    char *body = res->body;
    char *content_len;
    if (body == NULL || strlen(body) == 0)
    {
        content_len = malloc(2 * sizeof(char));
        if (content_len == NULL)
        {
            perror("[-]malloc failed");
            return NULL;
        }
        strcpy(content_len, "0");
        return content_len;
    }

    size_t len = strlen(body);
    content_len = malloc(20 * sizeof(char));
    if (content_len == NULL)
    {
        perror("[-]malloc failed");
        return NULL;
    }
    sprintf(content_len, "%zu", len);
    return content_len;
}

/**
 * Validate the response
 * Add version (default 1,1), status_code (default 200) and content_length (calculate it) headers
 * Check if headers and content-type header is present
 * 
 * @param res a pointer to the response_t struct
 * @return 0 if the response is valid, -1 otherwise
*/
int validate_response(response_t *res)
{
    if (res->headers == NULL)
    {
        printf("[-]Headers not found\n");
        return -1;
    }
    if (res->version == NULL)
    {
        res->version = strdup("1.1");
        if (res->version == NULL)
        {
            perror("[-]malloc failed");
            return -1;
        }
    }
    if (res->status_code == NULL)
    {
        res->status_code = strdup("200");
        if (res->status_code == NULL)
        {
            perror("[-]malloc failed");
            return -1;
        }
    }

    if (get_header(res->headers, "Content-Type") == NULL)
    {
        printf("[-]Content-Type not found\n");
        return -1;
    }

    if (get_header(res->headers, "Content-Length") == NULL)
    {
        char *content_len = content_length(res);
        add_header(&(res->headers), "Content-Length", content_len);
        free(content_len);
    }
    return 0;
}

