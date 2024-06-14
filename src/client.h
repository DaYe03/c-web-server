/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/client.h
 * @brief provides the client_t struct and functions to manage clients
*/


#ifndef CLIENT_H
#define CLIENT_H

#include "http_data.h"
#include <pthread.h>

typedef struct client_t
{
    int client_fd;
    pthread_t thread_id;
    request_t *req;
    response_t *res;
    struct client_t *next;
} client_t;

extern void init_clients();

extern int add_client(client_t *client);

/**
 * Remove a client from the list
 * This function removes a client from the list.
 * 
 * @param client_fd the file descriptor of the client
*/
extern void remove_client(int client_fd);

/**
 * Get the head of the list
 * This function returns the head of the list.
 * 
 * @return a pointer to the head of the list
*/
extern client_t *get_client(int client_fd);

/**
 * Get the head of the list
 * This function returns the head of the list.
 * 
 * @return a pointer to the head of the list
*/
extern client_t *get_head();

/**
 * Free the list of clients
 * This function frees the memory allocated for the list of clients.
*/
extern void free_clients();

/**
 * Print the list of clients
 * This function prints the list of clients.
*/
extern void print_clients();

#endif