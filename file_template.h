// 
// File: file_template.h 
// Description: My take on a Unix V7 file structure using RAM disk in our operating system
//
// @author Gino Coppola
//
// References used for helping with making this project:
//      https://www.cs.rit.edu/~wrc/courses/common/notes/csci452/
//      https://learn.microsoft.com/en-us/troubleshoot/windows-client/backup-and-storage/fat-hpfs-and-ntfs-file-systems
//      https://github.com/v7unix/v7unix
//      https://www.tuhs.org/cgi-bin/utree.pl?file=V7
//      https://github.com/darshank15/Inode-based-file-system/blob/master/inode.h
//      https://www.tuhs.org/cgi-bin/utree.pl?file=V7/usr/include/sys
//      https://www3.nd.edu/~pbui/teaching/cse.30341.fa18/project06.html
//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#ifndef _FILE_TEMPLATE_H_
#define _FILE_TEMPLATE_H_

#include "cio.h"
#include "users.h"
#include "ulib.h"
#include "kdefs.h"
#include "kernel.h"

/*File address space start and end*/
#define USERLAND_FILE_ADDRESS_START 0xC0000001
#define USERLAND_FILE_ADDRESS_END 0xffffffff

/*Sizes for data blocks*/
#define MAX_NUM_OF_DATA_BLOCKS 960 // 15 pointers per inode and 64 inodes total
#define SIZE_OF_DATA_BLOCK_DEC 512
#define SIZE_OF_DATA_BLOCK_HEX 0x200

#define SIZE_OF_FILE_DEC 528
#define SIZE_OF_FILE_HEX 0x210

/*Sizes for inodes*/
#define TOTAL_NUM_OF_INODES 64
#define SIZE_OF_INODES_DEC 80
#define SIZE_OF_INODES_HEX 0x50
#define POINTERS_PER_INODE 15
#define SIZE_OF_DIRECTORY_NAME 16

/*Sizes for file system structure*/
#define SIZE_OF_FILE_SYSTEM_DEC 28 // In bytes
#define SIZE_OF_FILE_SYSTEM_HEX 0x1C

#define MAX_NAME_LENGTH 16

// Strucure for an inode
typedef struct Inode_s {
    uint8_t size;
    uint8_t num_of_pointers;
    bool_t is_direct; // Only the first block can be an indirect pointer, this will be true if it's direct, false if indirect
    void *direct[15]; // 1 node that can be either direct, or indirect
    char name[MAX_NAME_LENGTH]; // name of the directory (if is a directory)
} Inode_s; // Exactly 80 bytes

// Structure for a file
typedef struct File_s {
    char name[MAX_NAME_LENGTH];
    char data_block[SIZE_OF_DATA_BLOCK_DEC]; // pointer to 512 byte data block
} File_s; // 528 bytes

// Structure for the list of memory addresses
typedef struct list_s {
    // This will point to either a data section or another node
    // This will depend on what the mode byte is
    void *data;
    /*union data{
        File_s *file;
        Inode_s *inode;
    };*/
    struct list_s *next;
} list_s; 

// Structure for a file system
typedef struct FileSystem_s {
    uint32_t num_free_blocks;
    uint32_t num_free_nodes;
    list_s *free_nodes; // This will contain all of the free nodes
    list_s *used_nodes; // This will be empty at the initial process
    list_s *free_blocks; // This will contain all of the free blocks
    list_s *used_blocks; // This will be empty at the initial process
    Inode_s *current_inode; // The current inode (working directory)
    Inode_s *previous_inode; // The previous inode (one directory level up)
} FileSystem_s; // 28 bytes

/**
 * @brief Initialize all of the inode into memory. This will loop through one by one and add the 
 *        appropriate amount of inodes to the linked list in memory
 * 
 * @param fs - the file system structure
 * @param current_address - the variable that will store the address of the node to be allocated in
 * @return uint32_t - the next address that will be used by the data blocks init
 */
uint32_t init_inodes(FileSystem_s *fs, uint32_t current_address);

/**
 * @brief Initialize all of the data blocks into memory, similar to the way the data blocks do it
 * 
 * @param fs - the file system structure
 * @param current_address - the variable that will store the address of the node to be allocated in
 * @return uint32_t - the last address that was allocated for the data blocks
 */
uint32_t init_data_blocks(FileSystem_s *fs, uint32_t current_address);

/**
 * @brief Call the inits for the inodes and the data blocks, as well as setting the counts and current/
 *        previous node pointers in the file system structure
 * 
 * @return FileSystem_s* - the baseline file system that will be used for the rest of the run time
 */
FileSystem_s* file_system_init (void);

/**
 * @brief When the user wants to create an inode, it will take a block off the free list and allocate it in the used list.
 *        I will possibly add a function to add an indirect pointer in, but not for now.
 * 
 * @param fs - the file system structure
 * @param name - the name of the inode if they want to create one
 * @param block - the block that will go in the inode if they make a direct one
 * @param index - the index into the array of pointers in the inode
 * @param is_direct - if the node will be direct or indirect
 * @param new_inode - the node if it is indirect
 * @param name - the name of the pointer, if it is direct (which it must be in this case to be made)
 * @return Inode_s* - the node that was created for the user and added into the file system
 */
Inode_s *create_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct, char name[SIZE_OF_DIRECTORY_NAME]);

/**
 * @brief Create a data block object and return it in the list object
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param is_direct - if the node being added is direct or indirect
 * @param name - the name of the file that will be created
 * @param block - the block if it is direct
 * @param index - the index to say where to add the node in
 * @return list_s* 
 */
File_s *create_data_block(FileSystem_s *fs, Inode_s *inode, bool_t is_direct, char name[14], char block[SIZE_OF_DATA_BLOCK_DEC], uint8_t index);

/**
 * @brief Read the data block from the current working directory. Check to make sure if the index is at 0, then it's direct
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to read from
 */
void inode_read (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number);

/**
 * @brief Send a data block to an inode in the current working directory
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to write into
 * @param block - the data block being sent into the inode
 */
void inode_write (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number, char *block);

/**
 * @brief 
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param index - the index of the inode
 */
void delete_inode_pointer(FileSystem_s *fs, Inode_s *inode, uint8_t index, char name[16]);

/**
 * @brief 
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param index - the index of the inode
 */
void delete_data_block_pointer(FileSystem_s *fs, Inode_s *inode, uint8_t index, char name[16]);

/**
 * @brief 
 * 
 * @param fs - the file system structure
 * @param inode - the inode the user wants to delete (if indirect)
 * @param index - the index of the direct pointer
 * @param is_direct - if the index is at 0, then check if it is direct or indirect
 */
void delete_pointer_in_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct, char name[16]);

/**
 * @brief Set the data block of an inode to be all spaces (empty characters) when the user wants
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to delete
 */
void inode_delete_data (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number);

/**
 * @brief Move into another directory by making sure the pointer at index 0 is a direct pointer
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @return Inode* - The current working directory we are in
 */
Inode_s* move_in_directory (FileSystem_s *fs, Inode_s *inode);

/**
 * @brief This will move to the last directory. The only issue is that it will lost the previous pointer
 * 
 * @param fs - the file system structure
 */
void move_out_directory (FileSystem_s *fs);

/**
 * @brief Print out info about the current working directory and the file system. This will include
 *        all pointers in the current inode that are not null, and will differentiate between the first
 *        one being a file or another directory (only first one can be).
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 */
void print_directory (FileSystem_s *fs, Inode_s *inode);

/**
 * @brief Print out the information of the file system such as the number of nodes in used, or freed.
 * 
 * @param fs - the file system structure
 */
void print_file_system_info(FileSystem_s *fs);

#endif
