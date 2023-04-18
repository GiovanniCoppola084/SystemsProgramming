#include "file_template.h"
#include "kmem.h"

void Filesystem_s* file_system_init (void) {
    Filesystem_s *fs = (Filesystem_s *) (USERLAND_FILE_ADDRESS_START + 1);

    // Set the initial number of blocks
    fs->num_free_blocks = MAX_NUM_OF_DATA_BLOCKS;
    // Set the initial number of free nodes
    fs->num_free_nodes = TOTAL_NUM_OF_INODES;

    // Add inode sector given the size of the inodes
    // This is the start plus the 16 bits for the file systems struct
    uint32_t current_address = USERLAND_FILE_ADDRESS_START +  SIZE_OF_FILE_SYSTEM + 1;

    // Set the free nodes to be the head of the list now
    fs->free_nodes = NULL;

    for (int i = 0; i < TOTAL_NUM_OF_INODES; i++) {
        list_s *new_node = (list_s *) current_address;
        assert(new_node != NULL);

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
    }

    // Set the used nodes to null since we wont be using any at init
    fs->used_nodes = NULL;

    // Move it up one byte plus the size of an inode to get into the block space
    current_address = current_address + SIZE_OF_INODE + 1;

    fs->free_blocks = NULL;

    for (int i = 0; i < TOTAL_NUM_OF_DATA_BLOCKS; i++) {
        list_s *new_node = (list_s *) current_address;
        assert(new_node != NULL);

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
    }

    fs->used_blocks = NULL;

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