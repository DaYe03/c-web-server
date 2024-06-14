/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/route.h
 * @brief provides functions to manage the routes
*/

#include "http_data.h"

#ifndef ROUTE_H
#define ROUTE_H

typedef void (*callback)(request_t *req, response_t *res);

typedef struct route {
    char *path;
    char *method;
    callback cb;

    struct route *next;
} route_t;

/**
 * Add a route
 * This function adds a route to the list of routes.
 * 
 * @param method the method of the route
 * @param path the path of the route
 * @param cb the callback of the route
 * @return 0 if the route is added, -1 otherwise
 */
extern int add_route(char *path, char *method, callback cb);

/**
 * Get a route
 * This function returns the callback of a route.
 * 
 * @param method the method of the route
 * @param path the path of the route
 * @return the callback of the route or NULL if the route is not found
 */
extern callback get_route(char *path, char *method);

/**
 * Print all routes
 * This function prints all the routes.
 */
extern void print_all();

#endif // ROUTE_H