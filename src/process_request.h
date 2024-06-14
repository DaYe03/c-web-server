/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/process_request.h
 * @brief provides functions to parse the request
*/

#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H

#include "http_data.h"

/**
 * Parse the first line of a request
 * This function parses the first line of a request and stores the method, path and version in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the first line of the request
 * @return 0 if the function was successful, -1 otherwise
*/
extern int parse_first_line(request_t *req, char *buffer);

/**
 * Parse the headers of a request
 * This function parses the headers of a request and stores them in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the headers of the request
 * @return 0 if the function was successful, -1 otherwise
*/
extern int parse_headers(request_t *req, char *buffer);

/**
 * Parse the body of a request
 * This function parses the body of a request and stores it in the request_t struct.
 * 
 * @param req a pointer to the request_t struct
 * @param buffer the buffer containing the body of the request
 * @param size the size of the body
 * @return 0 if the function was successful, -1 otherwise
 */
extern int parse_body(request_t *req, char *buffer, size_t size);

#endif // PROCESS_REQUEST_H