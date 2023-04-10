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

#define DATA_BLOCK_SIZE 64

/**
* 0 needs to be the root index for an inode
*/

typedef struct inode_s {
    // Needs to contain a list of what is within the directory
    // This is where block pointers are
    // File or direc?
    uint16_t i_nlink;
    uint32_t i_size;
    uint32_t i_number;
    unsigned char i_count;
} Inode_s; // 11 bytes

extern struct Ilist_s inode_s[]; // We want total number of Inodes to be disk size / address size (address?)

typedef struct filesys {
    uint16_t s_isize;
    uint16_t s_ninode;
    uint32_t s_inode[128]; // Free i-node list?
    uint32_t s_tinode; // Total i-nodes
    char s_fname[16]; // 16 characters for the file name
} Filesys_s; // 28 bytes

/**
* 64 bytes in total for the size of the file, where 48 bytes is the data in the file
*/
typedef struct file_s {
    /*unsigned char name[14]; // Name of the file
    unsigned char entry[2]; // Entry number into the inode
    unsigned char data[48]; // Data block*/
    struct inode_s *f_inode;
    unsigned char datablock[DATA_BLOCK_SIZE];
    unsigned char f_count;
} File_s; // 73 bytes

extern struct file_s file[]; // File table

struct direct {
    uint32_t d_ino;
    unsigned char d_name[DATA_BLOCK_SIZE]; // What is directory size?
};

/* Separate two different implementations */

typedef struct SuperBlock_s {
    uint32_t magic_number; // Do I need this?
    uint32_t num_of_inodes; // Number of i-nodes in the system
} SuperBlock_s;

typedef struct Inode_s {
    uint32_t size;
    unsigned char name[16];
    uint32_t direct[12]; // 12 direct pointers per i-node
    uint32_t indirect; // Indirect pointer
} Inode_s;

typedef struct File_s {
    SuperBlock_s super;
    Inode_s inodes[16]; // 16 i-node pointers per block
    uint32_t pointers[128]; // 64 pointers per block
    unsigned char data_block[DATA_BLOCK_SIZE]; // 64 bytes per data block
} File_s;

typedef struct FileSystem_s {
    uint32_t fd;
    uint32_t blocks; // Number of blocks
} FileSystem_s;

uint16_t create_file_system (FileSystem_s *fs);
void delete_file_system (FileSystem_s *fs, uint32_t inode_number);
void inode_size (FileSystem_s *fs, uint32_t inode_number);
void inode_read (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length);
void inode_write (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length);

#endif
