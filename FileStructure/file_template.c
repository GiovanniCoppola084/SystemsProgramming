#include "file_template.h"
#include "kmem.h"

void Filesystem_s* file_system_init (void) {
    Filesystem_s *fs = (Filesystem_s *) USERLAND_FILE_ADDRESS_START;

    // Set the initial number of blocks
    fs->num_free_blocks = TOTAL_NUM_BLOCKS;
    // Set the initial number of free nodes
    fs->num_free_nodes = TOTAL_NUM_NODE;

    // Add inode sector given the size of the inodes
    // This is the start plus the 16 bits for the file systems struct
    uint64_t start_of_inode_sector = USERLAND_FILE_ADDRESS_START +  SIZE_OF_FILE_SYSTEM

    // Go to beginning of the data block (size of inodes * num of inodes + base addr of inodes)
    for (int i = 0; i < )

    return fs;
}

/**
* Create the file system by allocating a slab for the entire thing (since we limit to 128 inodes)
*/
uint16_t create_file_system (FileSystem_s *fs) {
    fs = (FileSystem_s*) _km_page_alloc(1); // I do want to add to free block list since I cannot use kalloc without free blocks
    assert (fs != NULL);
    return fs;
}

/**
* Delete the entire file system by freeing up the memory
*/
void delete_file_system (FileSystem_s *fs, uint32_t inode_number) {
    assert1(fs != NULL);

    Inode_s *inode = (Inode_s *) fs->inode_pointers[inode_number];
    
    // Free the pointers
}

/**
* Get the size of the inode present
*/
int inode_size (FileSystem_s *fs, uint32_t inode_number) {
    Inode_s *inode = (Inode_s *) fs->inode_pointers[inode_number];

    return inode->size;
}

/**
* Get the inode by going into the list of inodes within the file system
*/
Inode_s* get_inode (FileSystem_s *fs, uint32_t inode_number) {
    return (Inode_s *) fs->inode_pointers[inode_number];
}

/**
* Output what is in the file system to a data block
*/
void inode_read (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length) {
    // First check to make sure the inode at the current index is not a direct pointer.
    // If it is, then we need to panic since it's trying to access a data block that is does not have.
    // If that check succeeds, then we just give the data block to the block passed into the function

    Inode_s* inode = get_inode (fs, inode_number);

}

/**
* Read what is in the data block to the file
*/
void inode_write (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length) {

}

Inode* move_in_directory (Filesystem_s *fs, uint32_t inode_number) {
    // Check if the inode at teh number is a directory or not (direct pointer or non-direct pointer)
}

Inode* mode_out_directory (FileSystem_s *fs, uint32_t inode_number) {

}