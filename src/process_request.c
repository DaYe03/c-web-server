/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/process_request.c
 * @brief implementation of process_request.h
*/

#include "process_request.h"
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * Check if a key is valid
 * This function checks if a key is valid, i.e. if it contains at least one non-whitespace character.
 *
 * @param key the key to check
 * @return 1 if the key is valid, 0 otherwise
 */
int is_valid_key(const char *key)
{
    while (*key != '\0')
    {
        if (!isspace((unsigned char)*key))
            return 1;
        key++;
    }
    return 0;
}

/**
 * Add queries to the request_t struct
 * 
*/
int process_query(request_t *req, char *buffer)
{
    int n_params = 0;

    char *end_path = strchr(buffer, '?');
    if (end_path == NULL)
        return 0;

    char *copy = strdup(end_path + 1);
    if (copy == NULL)
    {
        perror("[-]Error allocating memory");
        return -1;
    }

    req->path = strndup(buffer, end_path - buffer);

    char *token = strtok(copy, "&");

    while (token != NULL)
    {
        char *value = "", *end_key = strchr(token, '=');
        if (end_key != NULL)
        {
            value = strdup(end_key + 1);
            if (value == NULL)
            {
                perror("[-]Error allocating memory");
                free(copy);
                return -1;
            }
        }
        char *key = strndup(token, end_key - token);
        if (key == NULL)
        {
            perror("[-]Error allocating memory");
            free(copy);
            return -1;
        }
        if (!is_valid_key(key))
        {
            free(copy);
            free(key);
            return -1;
        }

        add_param(&(req->body.params), key, value);
        n_params++;
        token = strtok(NULL, "&");
        free(key);
        free(value);
    }
    req->body.n_params = n_params;
    free(copy);
    return 0;
}

/**
 * Parse the metadata of a request
 * This function parses the first line of a request and stores the method, path and version in the request_t struct.
 * Call process_query to parse the query string
 *
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the first line of the request
 * @return 0 if the function was successful, -1 otherwise
 */
int parse_first_line(request_t *req, char *buffer)
{
    char *copy = strdup(buffer);
    if (copy == NULL)
    {
        perror("[-]Error allocating memory");
        return -1;
    }
    regex_t regex;
    regmatch_t match[4];
    char *pattern = "([A-Z]+) ([^ ]+) HTTP/([0-9].[0-1])";
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        perror("[-]Error compiling regex");
        free(copy);
        return -1;
    }
    if (regexec(&regex, copy, 4, match, 0) != 0)
    {
        perror("[-]Error parsing invalid request line");
        regfree(&regex);
        free(copy);
        return -1;
    }
    req->method = strndup(copy + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
    req->path = strndup(copy + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
    req->version = strndup(copy + match[3].rm_so, match[3].rm_eo - match[3].rm_so);

    if (process_query(req, req->path) < 0)
    {
        perror("[-]Error processing query");
        regfree(&regex);
        free(copy);
        return -1;
    }

    regfree(&regex);
    free(copy);
    return 0;
}

/**
 * Parse the headers of a request
 * This function parses the headers of a request and stores them in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the headers of the request
 * @return 0 if the function was successful, -1 otherwise
*/
int parse_headers(request_t *req, char *buffer)
{
    char *copy = strdup(buffer);
    if (copy == NULL)
    {
        perror("[-]Error allocating memory");
        return -1;
    }
    char *token = strtok(copy, "\r\n");
    while (token != NULL)
    {
        regex_t regex;
        regmatch_t match[3];
        char *pattern = "([^:]+): (.+)";
        if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
        {
            perror("[-]Error compiling regex");
            free(copy);
            return -1;
        }
        if (regexec(&regex, token, 3, match, 0) != 0)
        {
            printf("token: %s\n", token);
            perror("[-]Error parsing request headers");
            regfree(&regex);
            free(copy);
            return -1;
        }
        char *key = strndup(token + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        char *value = strndup(token + match[2].rm_so, match[2].rm_eo - match[2].rm_so);

        add_header(&(req->headers), key, value);

        regfree(&regex);
        token = strtok(NULL, "\r\n");
    }
    free(copy);
    return 0;
}

int set_body(request_t *req, char *buffer, size_t size)
{
    req->body.data = (char *)malloc(size + 1);
    if (req->body.data == NULL)
    {
        perror("[-]Error setting body");
        return -1;
    }
    memcpy(req->body.data, buffer, size);
    req->body.data[size] = '\0';
    return 0;
}

/**
 * Process the json body of a request
 * This function processes the json body of a request and stores it in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the json body
 * @param size the size of the body
 * @return 0 if the function was successful, -1 otherwise
 */
int process_json(request_t *req, char *buffer, size_t size)
{
    int n_params = 0;
    char *copy = strdup(buffer);
    if (copy == NULL)
    {
        perror("[-]Error allocating memory");
        return -1;
    }
    // read copy line by line
    char *token = strtok(copy, "\r\n");
    while (token != NULL)
    {
        if (token[0] == '{' || token[0] == '}')
        {
            token = strtok(NULL, "\r\n");
            continue;
        }
        regex_t regex;
        regmatch_t match[3];
        char *pattern = "\"([^\"]+)\": \"([^\"]+)\"";
        if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
        {
            perror("[-]Error compiling regex");
            free(copy);
            return -1;
        }
        if (regexec(&regex, token, 3, match, 0) != 0)
        {
            perror("[-]Error processing json");
            regfree(&regex);
            free(copy);
            return -1;
        }
        char *key = strndup(token + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        char *value = strndup(token + match[2].rm_so, match[2].rm_eo - match[2].rm_so);

        add_param(&(req->body.params), key, value);
        n_params++;

        regfree(&regex);
        token = strtok(NULL, "\r\n");
        free(key);
        free(value);
    }
    req->body.n_params = n_params;
    free(copy);
    return 0;
}

/**
 * Process the form urlencoded body of a request
 * This function processes the form urlencoded body of a request and stores it in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the form urlencoded body
 * @param size the size of the body
 * @return 0 if the function was successful, -1 otherwise
*/
int process_urlencoded(request_t *req, char *buffer, size_t size)
{
    int n_params = 0;
    char *copy = strdup(buffer);
    if (copy == NULL)
    {
        perror("[-]Error allocating memory");
        return -1;
    }
    // read copy line by line
    char *token = strtok(copy, "&");
    while (token != NULL)
    {
        regex_t regex;
        regmatch_t match[3];
        char *pattern = "([^:]+)=(.+)";
        if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
        {
            perror("[-]Error compiling regex");
            free(copy);
            return -1;
        }
        if (regexec(&regex, token, 3, match, 0) != 0)
        {
            perror("[-]Error processing form urlencoded");
            regfree(&regex);
            free(copy);
            return -1;
        }
        char *key = strndup(token + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        char *value = strndup(token + match[2].rm_so, match[2].rm_eo - match[2].rm_so);

        add_param(&(req->body.params), key, value);
        n_params++;

        regfree(&regex);
        token = strtok(NULL, "&");
        free(key);
        free(value);
    }
    req->body.n_params = n_params;
    free(copy);
    return 0;
}

/**
 * Parse the body of a request
 * This function parses the body of a request and stores it in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the body of the request
 * @param size the size of the body
 * @return 0 if the function was successful, -1 otherwise
 */
int parse_body(request_t *req, char *buffer, size_t size)
{
    char *content = search_node(req->headers, "Content-Type")->value;
    // add deflation here
    if (strcmp(content, "application/json") == 0)
        if (process_json(req, buffer, size) < 0)
            return -1;
    if (strcmp(content, "application/x-www-form-urlencoded") == 0)
        if (process_urlencoded(req, buffer, size) < 0)
            return -1;
    if (set_body(req, buffer, size) < 0)
        return -1;

    return 0;
}