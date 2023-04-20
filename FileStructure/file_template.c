#include "file_template.h"
#include "kmem.h"

/**
* file_system_init - this function will initialize the file system by providing the free blocks
*                    that will be used for the inodes and the data blocks. The used blocks linked
*                    list will be null since we won't have any in use at init.
* 
* @return the file system will the initialized linked lists for memory
*/
Filesystem_s* file_system_init (void) {
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
        current_address = current_address + SIZE_OF_INODES + 1;
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
        current_address = current_address + SIZE_OF_DATA_BLOCK + 1;
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

void create_inode(FileSystem_s *fs, uint8_t num_pointers, bool_t is_direct) {
    assert(fs->free_nodes == NULL); // Assert that we still have free nodes. If we don't, then we panic
    
    list_s *new_node = fs->free_nodes;

    if (fs->used_nodes != NULL) {
        fs->used_nodes = new_node;
        fs->free_nodes = new_node->next;
    } else {
        list_s *current = fs->used_nodes;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;

        assert(fs->used_nodes != NULL);

        current = current->next;
        current->data = (Inode_s *) current->data;
        current->data->size = 0;
        current->data->num_of_pointers = 0;
        current->data->is_direct = false;
        for (int i = 0; i < 16; i++) {
            current->data->direct[i] = NULL;
        }

        fs->free_nodes = new_node->next;
    }

    fs->num_free_nodes--;
    fs->num_used_nodes++;
}

void create_data_block(FileSystem_s *fs, unsigned char *name, unsigned char *block, uint8_t index) {
    assert(fs->free_blocks != NULL);

    list_s *new_node = fs->free_blocks;

    if (fs->used_blocks != NULL) {
        fs->used_blocks = new_node;
        fs->free_blocks = new_node->next;
    } else {
        list_s *current = fs->used_blocks;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;

        assert(fs->used_nodes != NULL);

        current = current->next;
        current->data = (File_s *) current->data;
        current->data->name = name;
        current->data->file_index = index;
        current->data->data_block = block;

        fs->free_blocks = new_node->next;
    }

    fs->num_free_blocks--;
    fs->num_used_blocks++;
}

/**
* Output what is in the file system to a data block
*/
void inode_read (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length) {
    // This will be accessing the data block in a file, rather than accessing another inode
    // So if the mode bit tells you that it is a direct pointer, get the data block
    // If it's not, then we panic and abort
    
}

/**
* Read what is in the data block to the file
*/
void inode_write (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length) {

}

Inode* move_in_directory (Filesystem_s *fs, uint32_t inode_number) {
    
}

Inode* mode_out_directory (FileSystem_s *fs, uint32_t inode_number) {

}