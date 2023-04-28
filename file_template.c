/*
** SCCS ID: @(#)file_template.h	3/23/2023
**
** File:    file_template.c
**
** Authors: Gino Coppola
**
** Description: Function definitions for the Unix V7 based file system for our OS.
**              This file includes the methods to initialize the file system, create and
**              delete both inodes and data blocks, write/read from data blocks,
**              move into directories, and print information about the current 
**              working directory. 
** 
*/

#include "file_template.h"

FileSystem_s* file_system_init (void) {
    FileSystem_s *fs = (FileSystem_s *) (USERLAND_FILE_ADDRESS_START + 0x1);

    // Set the initial number of blocks
    // Implicit conversion from 'int' to 'uint8_t' (aka 'unsigned char') changes value from 960 to 192
    fs->num_free_blocks = MAX_NUM_OF_DATA_BLOCKS;
    // Set the initial number of free nodes
    fs->num_free_nodes = TOTAL_NUM_OF_INODES;

    // Add inode sector given the size of the inodes
    // This is the start plus the 16 bits for the file systems struct
    uint32_t current_address = USERLAND_FILE_ADDRESS_START +  SIZE_OF_FILE_SYSTEM_HEX + 0x1;

    // Set the free nodes to be the head of the list now
    fs->free_nodes = NULL;

    for (int i = 0; i < TOTAL_NUM_OF_INODES; i++) {
        // Can I assign the pointer like this?
        // This will only cast the pointer in that address. This might need to be fixed
        // It should operate like malloc
        list_s *new_node = (list_s *) current_address;

        if (new_node == NULL) {
            _kpanic("new node is null");
        }
        // assert(new_node != NULL);

        new_node->data = NULL;
        new_node->next = NULL;

        if (fs->free_nodes == NULL) {
            fs->free_nodes = new_node;
        } else {
            list_s *current = fs->free_nodes;
            while (current->next) {
                current = current->next;
            }
            current->next = new_node;
        }

        // Move to the next inode space
        current_address = current_address + SIZE_OF_INODES_HEX + 0x1;
    }

    // Set the used nodes to null since we wont be using any at init
    fs->used_nodes = NULL;

    // Move it up one byte plus the size of an inode to get into the block space
    // current_address = current_address + SIZE_OF_INODES_HEX + 0x1;

    fs->free_blocks = NULL;

    for (int i = 0; i < MAX_NUM_OF_DATA_BLOCKS; i++) {
        list_s *new_node = (list_s *) current_address;

        if (new_node == NULL) {
            _kpanic("new node is null");
        }
        // assert(new_node != NULL);

        new_node->data = NULL;
        new_node->next = NULL;

        if (fs->free_blocks == NULL) {
            fs->free_blocks = new_node;
        } else {
            list_s *current = fs->free_blocks;
            while (current->next) {
                current = current->next;
            }
            current->next = new_node;
        }
    
        // Move to the next data block space
        current_address = current_address + SIZE_OF_FILE_HEX + 0x1;
    }

    fs->used_blocks = NULL;

    fs->current_inode = NULL;
    fs->previous_inode = NULL;

    return fs;
}

Inode_s *create_inode(FileSystem_s *fs, uint8_t index, bool_t is_direct) {
    // assert(fs->free_nodes != NULL); // Assert that we still have free nodes. If we don't, then we panic
    // assert(strlen(name) <= 14);
    
    if (fs->free_nodes == NULL) {
        _kpanic("no free nodes");
    }

    list_s *new_node, *node;

    if (fs->used_nodes == NULL) {
        // Move the head of the free nodes list to the used node list, and make the next pointer NULL
        new_node = fs->free_nodes; // Set the new node to be a free node
        new_node->next = NULL; // Set the nodes next to be NULL since the list was already empty
        fs->used_nodes = new_node; // Set the used nodes to the new node
        fs->free_nodes = fs->free_nodes->next; // Move the free list down a pointer
    } else {
        /*new_node = (list_s *)(&fs->used_nodes);
        new_node = fs->used_nodes;
        head = fs->used_nodes;
        new_node->next = head;
        head = new_node;*/
        // Insert current into head of linked list
        list_s *temp_node;
        temp_node = fs->free_nodes; // head of free node list
        new_node = fs->free_nodes; // This will be the new head of the used_nodes
        temp_node = temp_node->next; // Move the free nodes down the list
        new_node->next = fs->used_nodes; // Set the new_node so it can become the head of the list
        fs->used_nodes = new_node; // Now make it the head of the list
    }

    node = (list_s *)(&fs->used_nodes);
    if (node == NULL) {
        _kpanic("The new inode created is NULL");
    }

    // This is printing C000000D as the new address. This is not correct since it is in the file system space now
    char str[80];
    sprint(str, "Address of new node: %08x\n", node);
    cwrites(str);

    // current = current->next;
    Inode_s *accessed_node = (Inode_s *)(&node->data);
    accessed_node->size = 5;
    accessed_node->num_of_pointers = 0;

    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        accessed_node->direct[i] = NULL;
    }

    fs->num_free_nodes = fs->num_free_nodes - 1;

    return accessed_node;
}

void delete_pointer_in_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct) {
    // Get the node and delete it from the list. We need to search by name since we don't have the lis
    // Once it's found, check if it's direct or indirect

    // assert(fs->free_blocks != NULL);
    if (fs->free_blocks == NULL) {
        _kpanic("no free nodes");
    }
    // assert(inode != NULL);
    if (inode == NULL) {
        _kpanic("no free nodes");
    }
    
    if (!is_direct) {
        inode->direct[index] = NULL;
        
        list_s *free_node = fs->free_nodes;
        list_s *used_node = fs->used_blocks;
        list_s *node = NULL;
        list_s *head = fs->used_blocks;
        bool_t found = false;

        while (used_node->next) {
            // Is this the right way to type cast in this case?
            if (((Inode_s *)(used_node->data)) == inode) {
                // free_node->next = used_node;
                found = true;
                break;
            }
            node = used_node;
            used_node = used_node->next;
        }

        if (found == false) {
            _kpanic("not found");
        }
        // assert(found == true);
        // assert(free_node->next != NULL);

        list_s *temp = node; // Set the temp variable
        temp->data = NULL; // Set the data in temp to NULL
        temp->next = free_node; // Set temps next variable to the head of the free list (now the new head)
        fs->free_nodes = temp; // Set the free nodes to the new list
        node->next = used_node->next; // Remove the node from the used blocks list

        // Update the number of pointers
        fs->num_free_nodes = fs->num_free_nodes + 1;
        inode->num_of_pointers = inode->num_of_pointers - 1;
    } else {
        inode->direct[index] = NULL;

        list_s *free_block = fs->free_blocks;
        while (free_block->next) {
            free_block = free_block->next;
        }

        list_s *free_block = fs->free_blocks;
        list_s *used_block = fs->used_blocks;
        list_s *node = NULL;
        bool_t found = false;
        while (used_block->next) {
            if (((Inode_s *)(used_block->data)) == inode) {
                found = true;
                break;
            }
            node = used_block;
            used_block = used_block->next;
        }

        if (found == false) {
            _kpanic("not found");
        }
        // assert(found == true);
        // assert(free_block->next != NULL);

        list_s *temp = node; // Set the temp variable
        temp->data = NULL; // Set the data in temp to NULL
        temp->next = free_block; // Set temps next variable to the head of the free list (now the new head)
        fs->free_blocks = temp; // Set the free nodes to the new list
        node->next = used_block->next; // Remove the node from the used blocks list

        // Update the number of pointers
        fs->num_free_blocks = fs->num_free_blocks + 1;
        inode->num_of_pointers = inode->num_of_pointers - 1;
    }
}

File_s *create_data_block(FileSystem_s *fs, Inode_s *inode, bool_t is_direct, char name[14], char block[SIZE_OF_DATA_BLOCK_DEC], uint8_t index) {
    if (fs->free_blocks == NULL) {
        _kpanic("no free blocks");
    }
    // assert(fs->free_blocks != NULL);

    if (index == 0) {
        if (!is_direct) {
            _kpanic("not direct");
        }
        // assert(is_direct)
    }

    list_s *new_node = fs->free_blocks;
    list_s *current;

    if (fs->used_blocks == NULL) {
        fs->used_blocks = new_node;
        fs->free_blocks = new_node->next;

        current = fs->used_blocks;
    } else {
        current = fs->used_blocks;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }

    if (current->next == NULL) {
        _kpanic("next current node is null");
    }
    // assert(current->next != NULL);

    current = current->next;
    File_s *accessed_node = (File_s *)(current->data);
    // accessed_node->name = name;
    strcpy(accessed_node->name, name);
    accessed_node->file_index = index;
    // accessed_node->data_block = block;
    strcpy(accessed_node->data_block, block);

    fs->free_blocks = new_node->next;

    fs->num_free_blocks = fs->num_free_blocks - 1;

    inode->direct[index] = current;
    inode->num_of_pointers = inode->num_of_pointers + 1;

    return accessed_node;
}

void inode_read (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    // This will be accessing the data block in a file, rather than accessing another inode
    // So if the mode bit tells you that it is a direct pointer, get the data block
    // If it's not, then we panic and abort
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
        // assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char str[600];
    sprint(str, "Data in the block: %s\n", file->data_block);
    cwrites(str);
}

void inode_write (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number, char block[SIZE_OF_DATA_BLOCK_DEC]) {
    // This will work similar to read. If the inode is not a direct pointer, then we panic
    // If it is direct, then we simply set the data block
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
        // assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    //file->data_block = block;
    strcpy(file->data_block, block);
    char str[80];
    sprint(str, "Data now in block: %s\n", file->data_block);
    cwrites(str);
}

void inode_delete_data (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
        // assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char empty_block[SIZE_OF_DATA_BLOCK_DEC];
    for (int i = 0; i < SIZE_OF_DATA_BLOCK_DEC - 1; i++) {
        empty_block[i] = ' ';
    }
    empty_block[SIZE_OF_DATA_BLOCK_DEC - 1] = '\0';
    
    //file->data_block = &empty_block[0];
    strcpy(file->data_block, empty_block);
}

Inode_s *move_in_directory (FileSystem_s *fs, Inode_s *inode) {
    char str[10];
    sprint(str, "%08x, %02x\n", inode, inode->is_direct);
    cwrites(str);

    if (inode->is_direct) {
        _kpanic("node is not a directory");
    }
    // assert(!inode->is_direct);

    fs->previous_inode = inode;
    fs->current_inode = (Inode_s *) inode->direct[0];

    if (fs->current_inode == NULL) {
        _kpanic("the current inode is not null");
    }
    // assert(fs->current_inode != NULL);

    // Return the first pointer if it's not direct, since that will move us to a new directory
    return fs->current_inode;
}

void print_directory (FileSystem_s *fs, Inode_s *inode) {
    char str[80];
    sprint(str, "Current working director: \n");
    cwrites(str);
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        if (i == 0 && !inode->is_direct && inode->direct[0] != NULL) {
            sprint(str, "1. Directory");
            cwrites(str);
        } else if (i == 0 && inode->is_direct && inode->direct[0] != NULL) {
            File_s *file = (File_s *)inode->direct[0];
            sprint(str, "1. File: %s", file->name);
            cwrites(str);
        }

        if (i > 0 && inode->direct[i] != NULL) {
            File_s *file = (File_s *)inode->direct[i];
            sprint(str, "%d. File: %s", (i + 1), file->name);
            cwrites(str);
        }
    }

    sprint(str, "Total inodes in use: %d\n", (TOTAL_NUM_OF_INODES - fs->num_free_nodes));
    cwrites(str);
    sprint(str, "Total free inodes: %d\n", fs->num_free_nodes);
    cwrites(str);
    sprint(str, "Total blocks in use: %d\n", (MAX_NUM_OF_DATA_BLOCKS - fs->num_free_blocks));
    cwrites(str);
    sprint(str, "Total free blocks: %d\n", fs->num_free_blocks);
    cwrites(str);
}

// Might not be able to implement this unless I do a doubly linked list, or a if I store the previous in the file 
// system structure
Inode_s *mode_out_directory (FileSystem_s *fs, Inode_s *inode);
