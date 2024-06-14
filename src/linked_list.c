/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/linked_list.c
 * @brief implementation of linked_list.h
*/

#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Create a new node
 * This function allocates memory for a new node_t struct and copies the parameters into its fields.
 * 
 * @param key the key of the node
 * @param value the value of the node
 * @return a pointer to the node_t struct
 * @return NULL if an error occurred
*/
node_t *create_node(char *key, char *value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("[-]Error malloc");
        return NULL;
    }
    new_node->value = strdup(value);
    new_node->key = strdup(key);
    new_node->next = NULL;
    if (new_node->key == NULL || new_node->value == NULL) {
        perror("[-]Error strdup");
        free(new_node);
        return NULL;
    }
    return new_node;
}

/**
 * Insert a new node at the beginning of the list
 * This function creates a new node and inserts it at the beginning of the list.
 * 
 * @param head a pointer to the head of the list
 * @param key the key of the node
 * @param value the value of the node
 * @return 0 if the node was inserted successfully
 * @return -1 if an error occurred
*/
int insert_node(node_t **head, char *key, char *value) {
    node_t *new_node = create_node(key, value);
    if (new_node == NULL) {
        return -1;
    }
    new_node->next = *head;
    *head = new_node;
    return 0;
}

/**
 * Delete a node from the list
 * This function deletes a node from the list given its key.
 * 
 * @param head a pointer to the head of the list
 * @param key the key of the node to delete
 * @return 0 if the node was deleted successfully
 * @return -1 if the node was not found
*/
int delete_node(node_t **head, char *key) {
    if (head == NULL || *head == NULL) {
        return -1; // List is empty or head is NULL
    }
    node_t *current = *head;
    node_t *prev = NULL;
    while (current != NULL) {
        if (current->key != NULL && strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return -1;
}

/**
 * Search for a node in the list
 * This function searches for a node in the list given its key.
 * 
 * @param head a pointer to the head of the list
 * @param key the key of the node to search
 * @return a pointer to the node_t struct
 * @return NULL if the node was not found
*/
node_t * search_node(node_t *head, char *key) {
    node_t *current = head;
    while (current != NULL) {
        if (current->key != NULL && strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Free the list
 * This function frees the memory allocated for the list.
 * 
 * @param head a pointer to the head of the list
*/
void free_list(node_t *head) {
    node_t *current = head;
    node_t *next;
    while (current != NULL) {
        next = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = next;
    }
    head = NULL;
}

/**
 * Print the list
 * This function prints the key-value pairs of the list.
 * 
 * @param head a pointer to the head of the list
*/
void print_list(node_t *head) {
    node_t *current = head;
    while (current != NULL) {
        printf("%s:%s\n", current->key, current->value);
        current = current->next;
    }
}
