/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/process_response.h
 * @brief provides functions to process the response
*/

#ifndef PROCESS_RESPONSE_H
#define PROCESS_RESPONSE_H

#include "http_data.h"

/**
 * Get the status message
 * This function returns the status message corresponding to the status code.
 * 
 * @param res a pointer to the response_t struct
 * @return the status message or "Unknown" if the status code is not recognized
 */
extern char* get_status_message(response_t *res);

/**
 * Convert the headers to a string
 * This function converts the headers of a response to a string.
 * 
 * @param res a pointer to the response_t struct
 * @return a string containing the headers
 */
extern char* headers_to_string(response_t *res);

/**
 * Get the content length
 * This function returns the content length of the response body.
 * 
 * @param res a pointer to the response_t struct
 * @return the content length or "0" if the body is NULL
 */
extern char* content_length(response_t *res);

/**
 * Validate the response
 * Add version (default 1,1), status_code (default 200) and content_length (calculate it) headers
 * Check if headers and content-type header is present
 * 
 * @param res a pointer to the response_t struct
 * @return 0 if the response is valid, -1 otherwise
*/
extern int validate_response(response_t *res);

#endif // PROCESS_RESPONSE_H