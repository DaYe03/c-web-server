/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/server.h
 * @brief provides the server_t struct and functions to start and stop the server
*/

#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <netinet/in.h>
#include "route.h"
#include "http_data.h"

typedef struct{
    int server_fd;
    int port;
    int max_connections;
    struct sockaddr_in server_addr;
    pthread_t thread_id;
} server_t;

extern server_t * start_daemon(int port, int max_connections, const char *ip);
extern void stop_daemon(server_t * server);

#endif  // SERVER_H