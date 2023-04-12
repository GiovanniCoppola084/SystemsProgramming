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

#define TOTAL_NUM_BLOCKS 368426240
#define TOTAL_NUM_NODE 512
#define INODES_PER_FILE 128
#define DATA_BLOCK_SIZE 512

//*********************************************************************************************************************
//Separate two different implementations 
//*********************************************************************************************************************


/*typedef struct SuperBlock_s {
    uint32_t num_of_active_inodes;
    uint32_t num_of_inactive_inodes;
    uint32_t num_of_active_data_blocks;
    uint32_t num_of_inactive_data_blocks; // Can I convert this to a linked list of free data blocks?
    uint32_t magic_number; // This needs to be where it will start in memory
    unsigned char name[4]; // GFS
} SuperBlock_s; // 4 bytes*/

typedef struct Inode_s {
    uint32_t size;
    uint32_t direct[16]; // 16 pointers per i-node
    uint16_t num_of_pointers;
    bool_t mode; // True for direct, false for indirect
    unsigned char padding[1]; // 72 bytes
} Inode_s;

typedef struct File_s {
    unsigned char name[14];
    unsigned char index[2]; // 2 bytes for the file name
    uint32_t inodes[INODES_PER_FILE]; // 16 i-node pointers per block
    unsigned char data_block[DATA_BLOCK_SIZE]; // 512 bytes per data block
} File_s;

typedef struct FileSystem_s {
    uint8_t num_free_blocks;
    uint32_t free_blocks[TOTAL_NUM_BLOCKS]; // 368426240 total free blocks with the 1 Gb I will use
    uint8_t num_free_nodes;
    Inode_s free_nodes[TOTAL_NUM_NODE]; // 36,864 bytes taken up by Inodes in file system. 1,073,704,960 left from the one Gb I will use
} FileSystem_s;

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

#endif
