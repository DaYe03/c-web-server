/*!
 * c web server
 * Copyright (c) 2024 Daniele Ye <daniele.ye03@gmail.com>
 * MIT Licensed
*/

/**
 * @file lib/linked_list.h
 * @brief provides the node_t struct and functions to manage linked lists
*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node_t
{
    char *key;
    char *value;
    struct node_t *next;
}node_t;

/**
 * Create a new node
 * This function allocates memory for a new node_t struct and copies the parameters into its fields.
 * 
 * @param key the key of the node
 * @param value the value of the node
 * @return a pointer to the node_t struct
 * @return NULL if an error occurred
*/
extern node_t *create_node(char *key, char *value);

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
extern int insert_node(node_t **head, char *key, char *value);

/**
 * Delete a node from the list
 * This function deletes a node from the list given its key.
 * 
 * @param head a pointer to the head of the list
 * @param key the key of the node to delete
 * @return 0 if the node was deleted successfully
 * @return -1 if the node was not found
*/
extern int delete_node(node_t **head, char *key);

/**
 * Search for a node in the list
 * This function searches for a node in the list given its key.
 * 
 * @param head a pointer to the head of the list
 * @param key the key of the node to search
 * @return a pointer to the node_t struct
 * @return NULL if the node was not found
*/
extern node_t *search_node(node_t *head, char *key);

/**
 * Free the list
 * This function frees the memory allocated for the list.
 * 
 * @param head a pointer to the head of the list
*/
extern void free_list(node_t *head);

/**
 * Print the list
 * This function prints the key-value pairs of the list.
 * 
 * @param head a pointer to the head of the list
*/
extern void print_list(node_t *head);


#endif // LINKED_LIST_H