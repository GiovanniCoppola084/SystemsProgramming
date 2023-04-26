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
    
        // Move to the next data block space
        current_address = current_address + SIZE_OF_FILE_HEX + 0x1;
    }

    fs->used_blocks = NULL;

    fs->current_inode = NULL;
    fs->previous_inode = NULL;

    return fs;
}

Inode_s *create_inode(FileSystem_s *fs, char *name, char *block, uint8_t index, bool_t is_direct, Inode_s *new_inode) {
    assert(fs->free_nodes != NULL); // Assert that we still have free nodes. If we don't, then we panic

    list_s *new_node = fs->free_nodes;
    list_s *current;

    if (fs->used_nodes == NULL) {
        fs->used_nodes = new_node;
        fs->free_nodes = new_node->next;

        current = fs->used_nodes;
    } else {
        current = fs->used_nodes;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }

    assert(current->next != NULL);

    current = current->next;
    Inode_s *accessed_node = (Inode_s *)(current->data);
    accessed_node->size = 0;
    accessed_node->num_of_pointers = 0;

    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        accessed_node->direct[i] = NULL;
    }

    fs->free_nodes = new_node->next;

    fs->num_free_nodes--;

    Inode_s *node = (Inode_s *)(current->data);
    node->num_of_pointers = 0;
    return node;
}

void delete_pointer_in_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct) {
    assert(fs->free_blocks != NULL);
    assert(inode != NULL);
    
    if (is_direct) {
        inode->direct[index] = NULL;
        
        list_s *free_node = fs->free_nodes;
        while (free_node->next) {
            free_node = free_node->next;
        }

        list_s *used_node = fs->used_blocks;
        bool_t found = false;
        while (used_node->next) {
            // Is this the right way to type cast in this case?
            if (((Inode_s *)(used_node->data)) == inode) {
                free_node->next = used_node;
                found = true;
                break;
            }
        }

        assert(found == true); 
        assert(free_node->next != NULL);

        free_node = free_node->next;
        free_node->data = NULL;
        free_node->next = NULL;
        fs->num_free_nodes++;
        inode->num_of_pointers--;
    } else {
        inode->direct[index] = NULL;

        list_s *free_block = fs->free_blocks;
        while (free_block->next) {
            free_block = free_block->next;
        }

        list_s *used_block = fs->used_blocks;
        bool_t found = false;
        while (used_block->next) {
            if (((Inode_s *)(used_block->data)) == inode) {
                free_block->next = used_block;
                found = true;
                break;
            }
        }

        assert(found == true);
        assert(free_block->next != NULL);

        free_block = free_block->next;
        free_block->data = NULL;
        free_block->next = NULL;
        fs->num_free_blocks++;
        inode->num_of_pointers--;
    }
}

list_s *create_data_block(FileSystem_s *fs, Inode_s *inode, bool_t is_direct, char name[14], char block[512], uint8_t index) {
    assert(fs->free_blocks != NULL);
    if (index == 0) {
        assert(is_direct);
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

    assert(current->next != NULL);

    current = current->next;
    File_s *accessed_node = (File_s *)(current->data);
    //accessed_node->name = name;
    strcpy(accessed_node->name, name);
    accessed_node->file_index = index;
    //accessed_node->data_block = block;
    strcpy(accessed_node->data_block, block);

    fs->free_blocks = new_node->next;

    fs->num_free_blocks--;

    inode->direct[index] = current;
    inode->num_of_pointers++;

    return current;
}

void inode_read (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    // This will be accessing the data block in a file, rather than accessing another inode
    // So if the mode bit tells you that it is a direct pointer, get the data block
    // If it's not, then we panic and abort
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    sprint("Data in the block: %s", file->data_block);
}

void inode_write (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number, char block[SIZE_OF_DATA_BLOCK_DEC]) {
    // This will work similar to read. If the inode is not a direct pointer, then we panic
    // If it is direct, then we simply set the data block
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    //file->data_block = block;
    strcpy(file->data_block, block);
    char str[80];
    sprint(str, "Data now in block: %s", file->data_block);
    cwrites(str);
}

void inode_delete_data (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    if (inode_number == 0) {
        assert(inode->is_direct);
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char empty_block[SIZE_OF_DATA_BLOCK_DEC];
    for (int i = 0; i < SIZE_OF_DATA_BLOCK_DEC; i++) {
        empty_block[i] = ' ';
    }
    
    //file->data_block = &empty_block[0];
    strcpy(file->data_block, empty_block);
}

Inode_s *move_in_directory (FileSystem_s *fs, Inode_s *inode) {
    assert(!inode->is_direct);

    fs->previous_inode = inode;
    fs->current_inode = (Inode_s *) inode->direct[0];

    assert(fs->current_inode != NULL);

    // Return the first pointer if it's not direct, since that will move us to a new directory
    return fs->current_inode;
}

void print_directory (FileSystem_s *fs, Inode_s *inode) {
    char str[80];
    sprint(str, "Current working director: \n");
    cwrites(str);
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        if (i == 0 && !inode->is_direct && inode->direct[0] != NULL) {
            sprint(str, "1. Directory: %s");
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