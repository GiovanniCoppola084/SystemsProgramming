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

#define USERLAND_FILE_ADDRESS_START 0xC0000000
#define USRELAND_FILE_ADDRESS_END 0xffffffff

#define SIZE_OF_FILE_SYSTEM 0x10
#define SIZE_OF_INODE 0x48 // 72 bytes
#define SIZE_OF_FILE 0x320 // 560 bytes
#define TOTAL_NUM_BLOCKS 368426240
#define TOTAL_NUM_NODE 512
#define INODES_PER_FILE 128
#define DATA_BLOCK_SIZE 512

typedef struct Inode_s {
    uint32_t size;
    uint32_t direct[16]; // 16 pointers per i-node
    uint16_t num_of_pointers;
    bool_t mode; // True for direct, false for indirect
    unsigned char padding[1]; // 72 bytes
} Inode_s;

typedef struct File_s {
    unsigned char name[14];
    unsigned char index[2]; // 2 bytes for the file index
    uint32_t inodes[INODES_PER_FILE]; // 16 i-node pointers per block
    unsigned char data_block[DATA_BLOCK_SIZE]; // 512 bytes per data block
} File_s;

// Don't want file system as struct
// Data block is linked list that points to next
// I can point into the next block of memory when I need it (first/last (doesn't matter which) 32 bits are pointer)
// Take the first one off the free list when I need it
// Add the new one to the top of the free list when I am done with it
typedef struct FileSystem_s {
    uint8_t num_free_blocks;
    uint8_t num_free_nodes;
} FileSystem_s;

uint32_t free_blocks[TOTAL_NUM_BLOCKS]; // 368426240 total free blocks with the 1 Gb I will use
uint32_t free_nodes[TOTAL_NUM_NODE]; // 36,864 bytes taken up by Inodes in file system. 1,073,704,960 left from the one Gb I will use

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
* Get the size of the inode present
*/
int inode_size (FileSystem_s *fs, uint32_t inode_number);

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
