/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/http_data.h
 * @brief provides the request_t and response_t structs and functions to manage http data
*/

#ifndef HTTP_DATA_H
#define HTTP_DATA_H

#include "linked_list.h"
#include <stddef.h>

typedef struct 
{
    char *method;
    char *path;
    char *version;
    node_t *headers;
    struct
    {
        char *data;
        node_t *params;
        size_t n_params;
    }body;
}request_t;

typedef struct
{
    char *version;
    char *status_code;
    node_t *headers;
    char *body;
}response_t;

/**
 * Initialize a request_t struct
 * This function allocates memory for a new request_t struct and initializes its fields to NULL.
 * 
 * @return a pointer to the request_t struct
 * @return NULL if an error occurred
*/
extern request_t * init_request();

/**
 * Initialize a response_t struct
 * This function allocates memory for a new response_t struct and initializes its fields to NULL.
 * 
 * @return a pointer to the response_t struct
 * @return NULL if an error occurred
*/
extern response_t * init_response();

/**
 * Free a request_t struct
 * This function frees the memory allocated for a request_t struct.
 * 
 * @param req a pointer to the request_t struct
*/
extern void free_request(request_t *req);

/**
 * Free a response_t struct
 * This function frees the memory allocated for a response_t struct.
 * 
 * @param res a pointer to the response_t struct
*/
extern void free_response(response_t *res);

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
extern char *get_header(node_t *headers, char *key);

/**
 * Add a header
 * This function adds a header to a list of headers.
 * 
 * @param headers a pointer to the list of headers
 * @param key the key of the header
 * @param value the value of the header
*/
extern void add_header(node_t **headers, char *key, char *value);

/**
 * Add a parameter
 * This function adds a parameter to a list of parameters.
 * 
 * @param params a pointer to the list of parameters
 * @param key the key of the parameter
 * @param value the value of the parameter
*/
extern void add_param(node_t **params, char *key, char *value);

/**
 * Add a body
 * This function adds a body to a request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param body the body to add
*/
extern void add_body_res(response_t *res, char *body);

/**
 * Set the public path
 * This function sets the public path where the server will look for files.
 * 
 * @param path the public path
*/
extern void set_public_path(char *path);

/**
 * Add a file body
 * This function adds the content of a file to a response_t struct.
 * 
 * @param res a pointer to the response_t struct
 * @param path the path of the file
*/
extern void add_file_body(response_t *res, char *path);

/**
 * Add a version
 * This function adds a version to a response_t struct.
 * 
 * @param res a pointer to the response_t struct
 * @param version the version to add
*/
extern void add_version_res(response_t *res, char *version);

/**
 * Add a status code
 * This function adds a status code to a response_t struct.
 * 
 * @param res a pointer to the response_t struct
 * @param status_code the status code to add
*/
extern void add_status_code_res(response_t *res, char *status_code);

/**
 * Print a request
 * This function prints the fields of a request_t struct.
 * 
 * @param req a pointer to the request_t struct
*/
extern void print_request (request_t *req);

/**
 * Print a response
 * This function prints the fields of a response_t struct.
 * 
 * @param res a pointer to the response_t struct
*/
extern void print_response (response_t *res);

#endif //HTTP_DATA_H