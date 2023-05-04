// 
// File: file_template.h 
// Description: My take on a Unix V7 file structure using RAM disk in our operating system
//
// @author Gino Coppola
//
// Description: Function definitions for the Unix V7 based file system for our OS.
//              This file includes the methods to initialize the file system, create and
//              delete both inodes and data blocks, write/read from data blocks,
//              move into directories, and print information about the current 
//              working directory. 
// 
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#include "file_template.h"

/**
 * @brief Initialize all of the inode into memory. This will loop through one by one and add the 
 *        appropriate amount of inodes to the linked list in memory
 * 
 * @param fs - the file system structure
 * @param current_address - the variable that will store the address of the node to be allocated in
 * @return uint32_t - the next address that will be used by the data blocks init
 */
uint32_t init_inodes(FileSystem_s *fs, uint32_t current_address) {
    // Add inode sector given the size of the inodes
    // This is the start plus the 16 bits for the file systems struct
    current_address = USERLAND_FILE_ADDRESS_START +  SIZE_OF_FILE_SYSTEM_HEX + 0x1;

    // Set the free nodes to be the head of the list now
    fs->free_nodes = NULL;

    for (int i = 0; i < TOTAL_NUM_OF_INODES; i++) {
        list_s *new_node = (list_s *) current_address;

        if (new_node == NULL) {
            _kpanic("new node is null");
        }

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
        current_address = current_address + SIZE_OF_INODES_HEX;
    }

    // Set the used nodes to null since we wont be using any at init
    fs->used_nodes = NULL;

    return current_address;
}

/**
 * @brief Initialize all of the data blocks into memory, similar to the way the data blocks do it
 * 
 * @param fs - the file system structure
 * @param current_address - the variable that will store the address of the node to be allocated in
 * @return uint32_t - the last address that was allocated for the data blocks
 */
uint32_t init_data_blocks(FileSystem_s *fs, uint32_t current_address) {
    fs->free_blocks = NULL;

    for (int i = 0; i < MAX_NUM_OF_DATA_BLOCKS; i++) {
        list_s *new_node = (list_s *) current_address;

        if (new_node == NULL) {
            _kpanic("new node is null");
        }

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
        current_address = current_address + SIZE_OF_FILE_HEX;
    }

    fs->used_blocks = NULL;

    return current_address;
}

/**
 * @brief Call the inits for the inodes and the data blocks, as well as setting the counts and current/
 *        previous node pointers in the file system structure
 * 
 * @return FileSystem_s* - the baseline file system that will be used for the rest of the run time
 */
FileSystem_s* file_system_init (void) {
    FileSystem_s *fs = (FileSystem_s *) (USERLAND_FILE_ADDRESS_START);

    // Set the initial number of blocks
    // Implicit conversion from 'int' to 'uint8_t' (aka 'unsigned char') changes value from 960 to 192
    fs->num_free_blocks = MAX_NUM_OF_DATA_BLOCKS;
    // Set the initial number of free nodes
    fs->num_free_nodes = TOTAL_NUM_OF_INODES;

    uint32_t current_address = init_inodes(fs, NULL);

    current_address = init_data_blocks(fs, current_address);

    fs->current_inode = NULL;
    fs->previous_inode = NULL;

    return fs;
}

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
Inode_s *create_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct, char name[SIZE_OF_DIRECTORY_NAME]) {
    if (fs->free_nodes == NULL) {
        _kpanic("no free nodes");
    }

    list_s *new_node;

    if (fs->used_nodes == NULL) {
        // Move the head of the free nodes list to the used node list, and make the next pointer NULL
        new_node = fs->free_nodes; // Set the new node to be a free node
        fs->free_nodes = fs->free_nodes->next; // Move the free list down a pointer
        new_node->next = NULL; // Set the nodes next to be NULL since the list was already empty
        fs->used_nodes = new_node; // Set the used nodes to the new node
    } else {
        // Insert current into head of linked list
        list_s *temp_node;
        temp_node = fs->free_nodes;
        new_node = fs->free_nodes->next;
        new_node->next = fs->used_nodes;
        fs->used_nodes = new_node;
        temp_node = temp_node->next;
    }

    list_s *node = (list_s *)(fs->used_nodes);
    if (node == NULL) {
        _kpanic("The new inode created is NULL\n");
    }

    Inode_s *accessed_node = (Inode_s *)(&node->data);
    accessed_node->num_of_pointers = 0;
    accessed_node->is_direct = false;

    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        accessed_node->direct[i] = NULL;
    }

    // Copy the name into the name field of the directory
    strcpy(accessed_node->name, name);

    fs->num_free_nodes = fs->num_free_nodes - 1;

    /* Print that the inode was created successfully */
    char str[80];
    sprint(str, "Inode created: %08x\n", accessed_node);
    cwrites(str);

    return accessed_node;
}

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
File_s *create_data_block(FileSystem_s *fs, Inode_s *inode, bool_t is_direct, char name[16], char block[SIZE_OF_DATA_BLOCK_DEC], uint8_t index) {
    if (fs->free_blocks == NULL) {
        _kpanic("no free blocks");
    }

    if (index == 0) {
        if (!is_direct) {
            _kpanic("not direct");
        
    }

    list_s *new_node;

    if (fs->used_blocks == NULL) {
        new_node = fs->free_blocks;
        fs->free_blocks = fs->free_blocks->next;
        new_node->next = NULL;
        fs->used_blocks = new_node;
    } else {
        list_s *temp_node;
        temp_node = fs->free_blocks;
        new_node = fs->free_blocks->next;
        new_node->next = fs->used_blocks;
        fs->used_blocks = new_node;
        temp_node = temp_node->next;
    }

    list_s *node = (list_s *)(fs->used_blocks);
    if (node == NULL) {
        _kpanic("The new block created is NULL\n");
    }

    File_s *accessed_node = (File_s *)(&node->data);
    strcpy(accessed_node->name, name);
    strcpy(accessed_node->data_block, block);

    fs->num_free_blocks = fs->num_free_blocks - 1;

    inode->num_of_pointers = inode->num_of_pointers + 1;

    inode->direct[index] = accessed_node;

    /* Print that the data block was created successfully */
    char str[80];
    sprint(str, "Data block created: %08x\n", inode->direct[index]);
    cwrites(str);
    DELAY(LONG * 20);

    return accessed_node;
}

/**
 * @brief Read the data block from the current working directory. Check to make sure if the index is at 0, then it's direct
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to read from
 */
void inode_read (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    // This will be accessing the data block in a file, rather than accessing another inode
    // So if the mode bit tells you that it is a direct pointer, get the data block
    // If it's not, then we panic and abort
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char str[600];
    sprint(str, "Data in the block: %s\n", file->data_block);
    cwrites(str);
}

/**
 * @brief Send a data block to an inode in the current working directory
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to write into
 * @param block - the data block being sent into the inode
 */
void inode_write (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number, char block[SIZE_OF_DATA_BLOCK_DEC]) {
    // This will work similar to read. If the inode is not a direct pointer, then we panic
    // If it is direct, then we simply set the data block
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    strcpy(file->data_block, block);
    char str[80];
    sprint(str, "Data now in block: %s\n", file->data_block);
    cwrites(str);
}

/**
 * @brief Delete an inode pointer and return it to the free list of memory
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param index - the index of the inode
 */
void inode_delete_data (FileSystem_s *fs, Inode_s *inode, uint32_t inode_number) {
    if (inode_number == 0) {
        if (!inode->is_direct) {
            _kpanic("not direct");
        }
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char empty_block[SIZE_OF_DATA_BLOCK_DEC];
    for (int i = 0; i < SIZE_OF_DATA_BLOCK_DEC - 1; i++) {
        empty_block[i] = ' ';
    }
    empty_block[SIZE_OF_DATA_BLOCK_DEC - 1] = '\0';
    
    strcpy(file->data_block, empty_block);

    cwrites("Data in block has been deleted!\n");
}

/**
 * @brief Delete a data block pointer and return it to the free list of memory
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param index - the index of the inode
 */
void delete_inode_pointer(FileSystem_s *fs, Inode_s *inode, uint8_t index, char name[16]) {        
    list_s *free_node = fs->free_nodes;
    list_s *used_node = fs->used_nodes;
    list_s *node = NULL;
    bool_t found = false;
    
    while (used_node->next) {
        Inode_s *new_node = (Inode_s *)(used_node->data);
        char s2[16];
        strcpy(s2, new_node->name);
        if (strcmp(name, s2) == 0) {
            found = true;
            break;
        }
        node = used_node;
        used_node = used_node->next;
    }

    if (found == false) {
        _kpanic("not found");
    }=

    list_s *temp = node; // Set the temp variable
    temp->data = NULL; // Set the data in temp to NULL
    temp->next = free_node; // Set temps next variable to the head of the free list (now the new head)
    fs->free_nodes = temp; // Set the free nodes to the new list
    node->next = used_node->next; // Remove the node from the used blocks list

    // Update the number of pointers
    fs->num_free_nodes = fs->num_free_nodes + 1;
    inode->num_of_pointers = inode->num_of_pointers - 1;
}

/**
 * @brief When the user wants to delete an object in the current inode directory, it will be added to the free list. This will
 *        work for an indirect or direct pointer
 * 
 * @param fs - the file system structure
 * @param inode - the inode the user wants to delete (if indirect)
 * @param index - the index of the direct pointer
 * @param is_direct - if the index is at 0, then check if it is direct or indirect
 */
void delete_data_block_pointer(FileSystem_s *fs, Inode_s *inode, uint8_t index, char name[16]) {
    list_s *free_block = fs->free_blocks;
    list_s *used_block = fs->used_blocks;
    list_s *node = NULL;
    bool_t found = false;

    char str[80];
    sprint(str, "Used blocks pointer: %08x\n", used_block);
    cwrites(str);
    DELAY(LONG * 20);

    while (used_block->next) {
        Inode_s *new_node = (Inode_s *)(used_block->data);
        char s2[16];
        strcpy(s2, new_node->name);

        if (strcmp(name, s2) == 0) {
            found = true;
            break;
        }

        node = used_block;
        used_block = used_block->next;
    }

    if (found == false) {
        _kpanic("not found");
    }

    list_s *temp = node; // Set the temp variable
    temp->data = NULL; // Set the data in temp to NULL
    temp->next = free_block; // Set temps next variable to the head of the free list (now the new head)
    fs->free_blocks = temp; // Set the free nodes to the new list
    node->next = used_block->next; // Remove the node from the used blocks list

    // Update the number of pointers
    fs->num_free_blocks = fs->num_free_blocks + 1;
    inode->num_of_pointers = inode->num_of_pointers - 1;
}

/**
 * @brief Set the data block of an inode to be all spaces (empty characters) when the user wants
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @param inode_number - the index of the data block to delete
 */
void delete_pointer_in_inode(FileSystem_s *fs, Inode_s *inode, uint8_t index, bool_t is_direct, char name[16]) {
    // Get the node and delete it from the list. We need to search by name since we don't have the lis
    // Once it's found, check if it's direct or indirect

    if (fs->free_blocks == NULL) {
        _kpanic("no free nodes");
    }
    if (inode == NULL) {
        _kpanic("Inode is null");
    }
    
    if (!is_direct) {
        delete_inode_pointer(fs, inode, index, name);
    } else {
        delete_data_block_pointer(fs, inode, index, name);
    }

    cwrites("Pointer in the inode has been deleted!\n");
}

/**
 * @brief Move into another directory by making sure the pointer at index 0 is a direct pointer
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 * @return Inode* - The current working directory we are in
 */
Inode_s *move_in_directory (FileSystem_s *fs, Inode_s *inode) {
    char str[20];
    sprint(str, "%08x, %c\n", inode, inode->is_direct);
    cwrites(str);

    if (inode->is_direct) {
        _kpanic("node is not a directory");
    }

    fs->previous_inode = inode;
    fs->current_inode = (Inode_s *)(inode->direct[0]);
    
    // Panic here. Something is wrong with the current inode
    if (fs->current_inode == NULL) {
        _kpanic("the current inode is null");
    }

    // Return the first pointer if it's not direct, since that will move us to a new directory
    return fs->current_inode;
}

/**
 * @brief This will move to the last directory. The only issue is that it will lost the previous pointer
 * 
 * @param fs - the file system structure
 */
void move_out_directory (FileSystem_s *fs) {
    if (fs->previous_inode != NULL) {
        Inode_s *temp = fs->previous_inode;
        fs->current_inode = temp;
        temp = NULL;
    }
}

/**
 * @brief Print out info about the current working directory and the file system. This will include
 *        all pointers in the current inode that are not null, and will differentiate between the first
 *        one being a file or another directory (only first one can be). For the file system, it will print
 *        the number of inodes in use, and not in use. It will do the same for data blocks too.
 * 
 * @param fs - the file system structure
 * @param inode - the current working directory we are in
 */
void print_directory (FileSystem_s *fs, Inode_s *inode) {
    char str[80];
    Inode_s *node;
    File_s *file;

    if (inode->is_direct) {
        cwrites("Is direct!\n");
    } else {
        sprint(str, "Directory info for %s:\n", inode->name);
        cwrites("Directory info for \n");
    }

    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        if (i == 0) {
            if (inode->is_direct) {
                node = (Inode_s *)(inode->direct[i]);
                if (node == NULL)  {
                    sprint(str, "NULL\n");
                } else {
                    sprint(str, "D %d. %s\n", (i+1), node->name);
                }
            } else {
                file = (File_s *)(inode->direct[i]);
                if (file == NULL) {
                    sprint(str, "NULL\n");
                } else {
                    sprint(str, "F %d. %s\n", (i+1), file->name);
                }
            }
        } else {
            file = (File_s *)(inode->direct[i]);
            if (file == NULL) {
                sprint(str, "NULL\n");
            } else {
                sprint(str, "F %d. %s\n", (i+1), file->name);
            }
        }

        cwrites(str);
        // Shorter delay than others, but still enough to be abel to see all files in directory
        DELAY(LONG * 5);

        /*if (i == 0 && !inode->is_direct && (Inode_s *)(inode->direct[0]) != NULL) {
            Inode_s *next_inode = (Inode_s *)(inode->direct[0]);
            sprint(str, "Directory: %s\n", next_inode->name);
            cwrites(str);
        } else if (i == 0 && inode->is_direct && inode->direct[0] != NULL) {
            File_s *file = (File_s *)(inode->direct[0]);
            sprint(str, "1. File: %s\n", file->name);
            cwrites(str);
        } else if (i > 0 && inode->direct[i] != NULL) {
            File_s *file = (File_s *)(inode->direct[i]);
            sprint(str, "%d. File: %s\n", (i + 1), file->name);
        } else {
            sprint(str, "%d. NULL\n", (i + 1));
            cwrites(str);
        }*/
    }
}

void print_file_system_info (FileSystem_s *fs) {
    char str[80];
    cwrites("File System Information:\n");
    sprint(str, "Total inodes in use: %d\n", (TOTAL_NUM_OF_INODES - fs->num_free_nodes));
    cwrites(str);
    sprint(str, "Total free inodes: %d\n", fs->num_free_nodes);
    cwrites(str);
    sprint(str, "Total blocks in use: %d\n", (MAX_NUM_OF_DATA_BLOCKS - fs->num_free_blocks));
    cwrites(str);
    sprint(str, "Total free blocks: %d\n", fs->num_free_blocks);
    cwrites(str);
    DELAY(LONG * 20);
}