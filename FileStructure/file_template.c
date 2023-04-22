#include "file_template.h"

/**
* file_system_init - this function will initialize the file system by providing the free blocks
*                    that will be used for the inodes and the data blocks. The used blocks linked
*                    list will be null since we won't have any in use at init.
* 
* @return the file system will the initialized linked lists for memory
*/
FileSystem_s* file_system_init (void) {
    FileSystem_s *fs = (FileSystem_s *) (USERLAND_FILE_ADDRESS_START + 0x1);

    // Set the initial number of blocks
    // Implicit coversion from 'int' to 'uint8_t' (aka 'unsigned char') changes value from 960 to 192
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
        current_address = current_address + SIZE_OF_INODES_HEX + 0x1;
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
    // current_address = current_address + SIZE_OF_INODES_HEX + 0x1;

    fs->free_blocks = NULL;

    for (int i = 0; i < MAX_NUM_OF_DATA_BLOCKS; i++) {
        list_s *new_node = (list_s *) current_address;
        current_address = current_address + SIZE_OF_DATA_BLOCK_HEX + 0x1;
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

Inode_s *create_inode(FileSystem_s *fs, unsigned char *name, unsigned char *block, uint8_t num_pointers, uint8_t index, bool_t is_direct, Inode_s *new_inode) {
    assert(fs->free_nodes != NULL); // Assert that we still have free nodes. If we don't, then we panic
    
    // Check and see if the node is going to be direct or not (if it's at position 0)
    // Also need to change this up so that it also allocates the data for the if case, not just else

    list_s *new_node = fs->free_nodes;

    if (fs->used_nodes == NULL) {
        fs->used_nodes = new_node;
        fs->free_nodes = new_node->next;

        list_s *current = fs->used_nodes;
    } else {
        list_s *current = fs->used_nodes;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }

    assert(current->next != NULL);

    current = current->next;
    current->data = (Inode_s *) current->data;
    current->data->size = 0;
    current->data->num_of_pointers = 0;

    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        current->data->direct[i] = NULL;
    }

    fs->free_nodes = new_node->next;

    fs->num_free_nodes--;
    fs->num_used_nodes++;

    return current;
}

void delete_pointer_in_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct) {
    assert(fs->free_blocks != NULL);
    assert(inode != NULL);
    
    if (is_direct) {
        inode->direct[index] = NULL;
        
        list_s *free_node = ls->free_nodes;
        while (free_node->next) {
            free_node = free_node->next;
        }

        list_s *used_node = ls->used_blocks;
        bool_t found = false;
        while (used_node->next) {
            // Is this the right way to type cast in this case?
            if (((Inode_s *) used_node->data) == inode) {
                current->next = used_node;
                found = true;
                break;
            }
        }

        assert(found == true); 
        assert(current->next != NULL);

        current = current->next;
        current->data = NULL;
        current->next = NULL;
        fs->num_free_nodes++;
        fs->num_used_nodes--;
    } else {
        inode->direct[index] = NULL;

        list_s *free_block = ls->free_blocks;
        while (free_block->next) {
            free_block = free_block->next;
        }

        list_s *used_block = ls->used_blocks;
        bool_t found = false;
        while (used_block->next) {
            if (((File_s *) used_block->data) == inode) {
                current->next = used_block;
                found = true;
                break;
            }
        }

        assert(found == true);
        assert(current->next != NULL);

        current = current->next;
        current->data = NULL;
        current->next = NULL;
        fs->num_free_blocks++;
        fs->num_used_blocks--;
    }
}

list_s *create_data_block(FileSystem_s *fs, unsigned char *name, unsigned char *block, uint8_t index) {
    assert(fs->free_blocks != NULL);

    list_s *new_node = fs->free_blocks;

    if (fs->used_blocks == NULL) {
        fs->used_blocks = new_node;
        fs->free_blocks = new_node->next;

        list_s *current = fs->used_blocks;
    } else {
        list_s *current = fs->used_blocks;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }

    assert(current->next != NULL);

    current = current->next;
    current->data = (File_s *) current->data;
    current->data->name = name;
    current->data->file_index = index;
    current->data->data_block = block;

    fs->free_blocks = new_node->next;

    fs->num_free_blocks--;
    fs->num_used_blocks++;

    return current;
}

/**
* Output what is in the file system to a data block
*/
void inode_read (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    // This will be accessing the data block in a file, rather than accessing another inode
    // So if the mode bit tells you that it is a direct pointer, get the data block
    // If it's not, then we panic and abort
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    printf("Data in the block: %s", file->data_block);
}

/**
* Read what is in the data block to the file
*/
void inode_write (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number, unsigned char *block) {
    // This will work similar to read. If the inode is not a direct pointer, then we panic
    // If it is direct, then we simply set the data block
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    file->data_block = block;
    printf("Data now in block: %s", file->data_block);
}

void inode_delete_data (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    unsigned char empty_block[SIZE_OF_DATA_BLOCK];
    for (int i = 0; i < SIZE_OF_DATA_BLOCK; i++) {
        empty_block[i] = ' ';
    }
    
    file->data_block = &empty_block[0];
}

Inode* move_in_directory (Inode_s *inode, boot_t is_direct) {
    assert(!inode->is_direct);

    // Return the first pointer if it's not direct, since that will move us to a new directory
    return (Inode_s *) inode->direct[0];
}


// Might not be able to implement this unless I do a doubly linked list, or a if I store the previous in the file 
// system structure
Inode* mode_out_directory (FileSystem_s *fs, uint32_t inode_number);