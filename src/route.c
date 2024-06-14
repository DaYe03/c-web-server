/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/route.c
 * @brief implementation of route.h
*/

#include "route.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ROUTES 100

static route_t *get;
static route_t *post;
static route_t *put;
static route_t *delete;
static route_t *patch;

/**
 * Insert a route
 * This function inserts a route in the list of routes.
 * 
 * @param head a pointer to the head of the list
 * @param path the path of the route
 * @param method the method of the route
 * @param cb the callback of the route
 * @return 0 if the route is inserted, -1 otherwise
 */
int insert_route(route_t **head, char *path, char *method, callback cb)
{
    route_t *new_node = (route_t *)malloc(sizeof(route_t));
    if (new_node == NULL) {
        return -1;
    }
    new_node->path = strdup(path);
    new_node->method = strdup(method);
    new_node->cb = cb;
    new_node->next = *head;
    *head = new_node;
    return 0;
}

/**
 * Add a route
 * This function adds a route to the list of routes.
 * 
 * @param method the method of the route
 * @param path the path of the route
 * @param cb the callback of the route
 * @return 0 if the route is added, -1 otherwise
 */
int add_route(char *method, char *path, callback cb)
{
    if (strcmp(method, "GET") == 0) {
        return insert_route(&get, path, "GET",  cb);
    } else if (strcmp(method, "POST") == 0) {
        return insert_route(&post, path, "POST",  cb);
    } else if (strcmp(method, "PUT") == 0) {
        return insert_route(&put, path, "PUT",  cb);
    } else if (strcmp(method, "DELETE") == 0) {
        return insert_route(&delete, path, "DELETE",  cb);
    } else if (strcmp(method, "PATCH") == 0) {
        return insert_route(&patch, path, "PATCH",  cb);
    }
    return -1;   
}

/**
 * Get a route
 * This function returns the callback of a route.
 * 
 * @param method the method of the route
 * @param path the path of the route
 * @return the callback of the route or NULL if the route is not found
 */
callback get_route(char *method, char *path)
{
    route_t *head = NULL;
    if (strcmp(method, "GET") == 0) {
        head = get;
    } else if (strcmp(method, "POST") == 0) {
        head = post;
    } else if (strcmp(method, "PUT") == 0) {
        head = put;
    } else if (strcmp(method, "DELETE") == 0) {
        head = delete;
    } else if (strcmp(method, "PATCH") == 0) {
        head = patch;
    }
    while (head != NULL) {
        if (strcmp(head->path, path) == 0) {
            return head->cb;
        }
        head = head->next;
    }
    return NULL;
}

/**
 * Print all routes
 * This function prints all the routes.
 */
void print_all()
{
    route_t *head = get;
    while (head != NULL) {
        printf("GET %s\n", head->path);
        head = head->next;
    }
    head = post;
    while (head != NULL) {
        printf("POST %s\n", head->path);
        head = head->next;
    }
    head = put;
    while (head != NULL) {
        printf("PUT %s\n", head->path);
        head = head->next;
    }
    head = delete;
    while (head != NULL) {
        printf("DELETE %s\n", head->path);
        head = head->next;
    }
    head = patch;
    while (head != NULL) {
        printf("PATCH %s\n", head->path);
        head = head->next;
    }	
}