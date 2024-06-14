/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/client.c
 * @brief implementation of client.h
*/

#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


static client_t *clients;

void init_clients()
{
    clients = NULL;
}

int add_client(client_t *client)
{
    if (client == NULL)
    {
        perror("[-]malloc failed");
        return -1;
    }
    if (clients == NULL)
        clients = client;
    else
    {
        client_t *current = clients;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = client;
    }
    return 0;
}

/**
 * Remove a client from the list
 * This function removes a client from the list.
 * 
 * @param client_fd the file descriptor of the client
*/
void remove_client(int client_fd)
{
    client_t *current = clients;
    client_t *prev = NULL;
    while (current != NULL)
    {
        if (current->client_fd == client_fd)
        {
            if (prev == NULL)
            {
                clients = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            close(current->client_fd);
            free_request(current->req); 
            free_response(current->res);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/**
 * Get a client from the list
 * This function returns a client from the list.
 * 
 * @param client_fd the file descriptor of the client
 * @return a pointer to the client_t struct or NULL if the client is not found
*/
client_t* get_client(int client_fd)
{
    client_t *current = clients;
    while (current != NULL)
    {
        if (current->client_fd == client_fd)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Get the head of the list
 * This function returns the head of the list.
 * 
 * @return a pointer to the head of the list
*/
client_t* get_head()
{
    return clients;
}

/**
 * Free the list of clients
 * This function frees the memory allocated for the list of clients.
*/
void free_clients()
{
    client_t *current = clients;
    while (current != NULL)
    {
        client_t *next = current->next;
        close(clients->client_fd);
        if (clients->thread_id != 0)
            pthread_cancel(clients->thread_id);
        free_request(current->req);
        free_response(current->res);
        free(current);
        current = next;
    }
    clients = NULL;
}

/**
 * Print the list of clients
 * This function prints the list of clients.
*/
void print_clients()
{
    client_t *current = clients;
    while (current != NULL)
    {
        printf("client_fd: %d - thread_id: %lu\n", current->client_fd, current->thread_id);
        current = current->next;
    }
}
