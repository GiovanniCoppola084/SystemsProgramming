#include "./file_template.h"

#define MAX_LINE_LENGTH 80

/**
 * @brief This function will read in the line entered by the user one character at a time.
 *        It will also print the prompt before entering the user information
 * 
 * @param buffer - the buffer that the data will be put in
 * @param length - the length of the input data
 * @param prompt - the prompt to print before getting the user information
 */
void read_line (char *buffer, uint32_t length, char *prompt);

/**
 * @brief This function will break the system if the file system has not been initialized before
 *        doing an operation on it
 * 
 */
void is_fs_initialized(void);

/**
 * @brief This function will break the system if the first inode has not been initialized before
 *        doing an operation on it
 * 
 */
void is_inode(void);

/**
 * @brief This function will print all of the options of the menu to the screen
 * 
 */
void print_menu(void);

/*
list_s *node = fs->free_nodes;
fs->free_nodes = fs->free_nodes->next;
node->next = NULL;
fs->used_nodes = node;

list_s *node = fs->free_nodes;
fs->free_nodes = fs->free_nodes->next;
node->next = fs->used_nodes;
fs->used_nodes = node;

void moveNodeToHead(struct Node** srcHeadRef, struct Node** destHeadRef, int position) {
    if (*srcHeadRef == NULL) {
        printf("Source linked list is empty.\n");
        return;
    }

    if (position <= 0) {
        printf("Invalid position.\n");
        return;
    }

    struct Node* current = *srcHeadRef;
    struct Node* prev = NULL;
    int count = 0;

    // Traverse to the node at the given position
    while (current != NULL && count != position) {
        prev = current;
        current = current->next;
        count++;
    }

    if (current == NULL) {
        printf("Invalid position.\n");
        return;
    }

    // Adjust the pointers to remove the node from the source list
    if (prev != NULL)
        prev->next = current->next;
    else
        *srcHeadRef = current->next;

    // Move the node to the head of the destination list
    current->next = *destHeadRef;
    *destHeadRef = current;
}

if (previous != NULL) {
    previous->next = current->next;
} else {
    fs->used_nodes = current->next;
}

current->next = fs->free_nodes;
fs->free_nodes = current;
*/