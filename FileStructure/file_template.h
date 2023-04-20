/*
** SCCS ID: @(#)file_template.h	3/23/2023
**
** File:    file_template.h
**
** Authors: Gino Coppola
**
** Description: The prototypes for functions to set up a basic file.
** Base of Unix V7 or Microsoft FAT
** https://www.cs.rit.edu/~wrc/courses/common/notes/csci452/
** https://learn.microsoft.com/en-us/troubleshoot/windows-client/backup-and-storage/fat-hpfs-and-ntfs-file-systems
** https://github.com/v7unix/v7unix
** https://www.tuhs.org/cgi-bin/utree.pl?file=V7
** https://github.com/darshank15/Inode-based-file-system/blob/master/inode.h
** https://www.tuhs.org/cgi-bin/utree.pl?file=V7/usr/include/sys
** https://www3.nd.edu/~pbui/teaching/cse.30341.fa18/project06.html
*/

#ifndef _FILE_TEMPLATE_H_
#define _FILE_TEMPLATE_H_

#include "cio.h"
#include "users.h"
#include "ulib.h"
#include "vector.h"

#define USERLAND_FILE_ADDRESS_START 0xC0000000
#define USERLAND_FILE_ADDRESS_END 0xffffffff
#define MAX_NUM_OF_DATA_BLOCKS 960
#define SIZE_OF_DATA_BLOCK 512
#define TOTAL_SIZE_OF_ALL_DATA_BLOCKS (MAX_NUM_OF_DATA_BLOCKS * TOTAL_SIZE_DATA_BLOCK)
#define TOTAL_NUM_OF_INODES 64
#define SIZE_OF_INODES 64
#define TOTAL_SIZE_FILE_SYS_STRUCT (1 + 1 + (4 * MAX_NUM_OF_DATA_BLOCKS) + (4 * TOTAL_NUM_OF_INODES))

typedef struct Inode_s {
    uint8_t size;
    uint8_t num_of_pointers;
    uint8_t is_direct; // True for direct, false for indirect. This is only for the first pointer
    File_s *direct[15]; // 15 pointers to other inodes per inode (MAX)
} Inode_s; // Exactly 64 bytes

typedef struct File_s {
    unsigned char name[14];
    uint8_t file_index; // 2 bytes for the file index
    // unsigned char data_block[DATA_BLOCK_SIZE]; // 512 bytes per data block
    // pointer to data block
    uint32_t *data_block; // pointer to 512 byte data block
} File_s;

// Don't want file system as struct
// Data block is linked list that points to next
// I can point into the next block of memory when I need it (first/last (doesn't matter which) 32 bits are pointer)
// Take the first one off the free list when I need it
// Add the new one to the top of the free list when I am done with it
typedef struct FileSystem_s {
    uint8_t num_free_blocks;
    uint8_t num_free_nodes;
    list_s *free_nodes; // This will contain all of the free nodes
    list_s *used_nodes; // This will be empty at the initial process
    list_s *free_blocks; // This will contain all of the free blocks
    list_s *used_blocks; // This will be empty at the initial process
} FileSystem_s; // 20 bytes

typedef struct list_s {
    // This will point to either a data section or another node
    // This will depend on what the mode byte is
    void *data;
    list_s *next;
} list_s;

/**
* Make an init function that will add the addresses to the free lists in the file system
*/
Filesystem_s* file_system_init (void);

/**
* Create the file system by allocating a slab for the entire thing (since we limit to 128 inodes)
*/
uint16_t create_file_system (FileSystem_s *fs);

/**
* Delete the entire file system by freeing up the memory
*/
void delete_file_system (FileSystem_s *fs, uint32_t inode_number);

/**
* Create the node by finding the first free node and add it to the used nodes list (on the end)
*/
void create_inode(FileSystem_s *fs, uint8_t num_pointers, bool_t is_direct);

void create_data_block(FileSystem_s *fs, unsigned char block[size], uint16_t size);

/**
* Find the node in the used list and remove it
*/
void delete_inode(FileSystem_s *fs, Inode_s inode);

/**
* Get the inode by going into the list of inodes within the file system
*/
Inode_s* get_inode (FileSystem_s *fs, uint32_t inode_number);

/**
* Output what is in the file system to a data block
*/
void inode_read (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length);

/**
* Read what is in the data block to the file
*/
void inode_write (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length);

Inode* move_in_directory (FileSystem_s *fs, uint32_t inode_number);

Inode* mode_out_directory (FileSystem_s *fs, uint32_t inode_number);

#endif
