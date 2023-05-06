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
    // This is the start plus the 28 bytes for the file systems struct
    current_address = USERLAND_FILE_ADDRESS_START +  SIZE_OF_FILE_SYSTEM_HEX + 0x1;

    // Set the free nodes to be the head of the list now
    fs->free_nodes = NULL;

    // Loop through all of the possible inodes
    for (int i = 0; i < TOTAL_NUM_OF_INODES; i++) {
        list_s *new_node = (list_s *) current_address;

        // Make sure we actually have a usable memory address
        if (new_node == NULL) {
            _kpanic("Something went wrong with creating a new inode");
        }

        // Set data and next to NULL since this on the end of the list
        new_node->data = NULL;
        new_node->next = NULL;

        // Directly add it to free nodes if it's empty, add to the end otherwise
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

    cwrites("File system initialized!\n");

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

    // Loop through all of the potential data blocks
    for (int i = 0; i < MAX_NUM_OF_DATA_BLOCKS; i++) {
        list_s *new_node = (list_s *) current_address;

        // Want to make sure we have a usable address
        if (new_node == NULL) {
            _kpanic("Something went wrong with creating a new data block");
        }

        // Set data and next to NULL since we are on the end of the list
        new_node->data = NULL;
        new_node->next = NULL;

        // Add to the free list if this is the first one. Add to the end otherwise
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

    // Set the number of free blocks to max since nothing is allocated yet
    fs->num_free_blocks = MAX_NUM_OF_DATA_BLOCKS;
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
Inode_s *create_inode(FileSystem_s *fs, Inode_s *inode, char name[SIZE_OF_DIRECTORY_NAME]) {
    if (fs->free_nodes == NULL) {
        _kpanic("No free nodes");
    }

    if (fs->used_nodes == NULL) {
        /*// Move the head of the free nodes list to the used node list, and make the next pointer NULL
        list_s *temp_node = fs->free_nodes;
        new_node = fs->free_nodes;
        new_node->next = NULL; 
        fs->used_nodes = new_node;
        temp_node = temp_node->next;*/
        list_s *node = fs->free_nodes;
        fs->free_nodes = fs->free_nodes->next;
        node->next = NULL;
        fs->used_nodes = node;
    } else {
        /*// Insert current into head of linked list
        // Check to make sure this does not lose the head of the list (this is possible)
        list_s *temp_node = fs->free_nodes;
        new_node = fs->free_nodes;
        new_node->next = fs->used_nodes;
        fs->used_nodes = new_node;
        temp_node = temp_node->next;*/
        list_s *node = fs->free_nodes;
        fs->free_nodes = fs->free_nodes->next;
        node->next = fs->used_nodes;
        fs->used_nodes = node;
    }

    list_s *node = (list_s *)(fs->used_nodes);
    // Panic if the node is NULL. Means the allocation went wrong
    if (node == NULL) {
        _kpanic("The new inode created is NULL");
    }

    // Set the data in the node now
    Inode_s *accessed_node = (Inode_s *)(&node->data);
    accessed_node->num_of_pointers = 0;
    accessed_node->is_direct = false;

    // Set all pointers to NULL since this is a new directory
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        accessed_node->direct[i] = NULL;
    }

    // Copy the name into the name field of the directory
    strcpy(accessed_node->name, name);

    fs->num_free_nodes = fs->num_free_nodes - 1;
    inode->num_of_pointers = inode->num_of_pointers + 1;

    // If the [current] inode is NULL, then assign it to the current node
    // Otherwise, set it to the first node in the direct pointer array
    if (inode == NULL) {
        fs->current_inode = accessed_node;
    } else {
        // This will always overwrite the first node if the user created it
        fs->current_inode->direct[0] = accessed_node;
    }

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
        _kpanic("No free blocks");
    }

    if (index < 0 || index == POINTERS_PER_INODE) {
        _kpanic("Non-existing pointer accessed");
    }

    if (index == 0 && !is_direct) {
        _kpanic("Not a data block");
    }

    if (fs->used_blocks == NULL) {
        /*list_s *temp_node = fs->free_blocks;
        new_node = fs->free_blocks;
        new_node->next = NULL;
        fs->used_blocks = new_node;
        temp_node = temp_node->next;*/
        list_s *node = fs->free_blocks;
        fs->free_blocks = fs->free_blocks->next;
        node->next = NULL;
        fs->used_blocks = node;
    } else {
        /*list_s *temp_node = fs->free_blocks;
        new_node = fs->free_blocks;
        new_node->next = fs->used_blocks;
        fs->used_blocks = new_node;
        temp_node = temp_node->next;*/
        list_s *node = fs->free_blocks;
        fs->free_blocks = fs->free_blocks->next;
        node->next = fs->used_blocks;
        fs->used_blocks = node;
    }

    list_s *node = (list_s *)(fs->used_blocks);
    // Panic if the node is NULL. Means that our allocation went wrong
    if (node == NULL) {
        _kpanic("The new block created is NULL\n");
    }

    // Set the data pointer now
    File_s *accessed_node = (File_s *)(&node->data);
    strcpy(accessed_node->name, name);
    strcpy(accessed_node->data_block, block);

    fs->num_free_blocks = fs->num_free_blocks - 1;

    inode->num_of_pointers = inode->num_of_pointers + 1;

    inode->direct[index] = accessed_node;

    // Set the current node to the accessed node since the move in directory is not working
    // fs->previous_inode = inode;
    // fs->current_inode = accessed_node;

    /* Print that the data block was created successfully */
    char str[80];
    sprint(str, "Data block created: %08x\n", inode->direct[index]);
    cwrites(str);

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
    // Panic if we are reading from a directory
    if (inode_number == 0 && !inode->direct) {
        _kpanic("Not a data block");
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    // Print the entire data block
    char str[SIZE_OF_DATA_BLOCK_DEC + 15];
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
    // Panic if we are reading from a directory
    if (inode_number == 0 && !inode->direct) {
        _kpanic("Not a data block");
    }

    File_s *file = (File_s *)inode->direct[inode_number];

    strcpy(file->data_block, block);
    char str[SIZE_OF_DATA_BLOCK_DEC + 15];
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
    // Panic if we are reading from a directory
    if (inode_number == 0 && !inode->direct) {
        _kpanic("Not a data block");
    }

    File_s *file = (File_s *) inode->direct[inode_number];

    char empty_block[SIZE_OF_DATA_BLOCK_DEC];

    // Set the first character to a null character, so it knows to stop printing
    // Not sure if this will work as intended, but we'll see
    empty_block[0] = '\0';
    
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
    list_s *current = fs->used_nodes;
    list_s *previous = NULL;
    bool_t found = false;

    // Set the node to NULL
    inode->direct[index] = NULL;

    // Loop through every element, including the first and check to see if it is the inode
    do {
        Inode_s *new_node = (Inode_s *)(&current->data);

        if (strcmp(name, new_node->name) == 0) {
            found = true;
            break;
        }

        // Previous node
        previous = current;
        // This is the current node
        current = current->next;
    } while(current);

    // Panic if we did not find the node (lost it somewhere or just a bad node)
    if (found == false || current == NULL) {
        _kpanic("Inode not found");
    }

    // Free the item by putting it on the free list and clearing data
    /*list_s *temp = node; 
    temp->data = NULL; 
    temp->next = free_node;
    fs->free_nodes = temp;
    node->next = used_node->next;*/
    if (previous != NULL) {
        previous->next = current->next;
    } else {
        fs->used_nodes = current->next;
    }

    current->next = fs->free_nodes;
    fs->free_nodes = current;

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
    list_s *current = fs->used_blocks;
    list_s *previous = NULL;
    bool_t found = false;

    // Set the node to NULL
    inode->direct[index] = NULL;

    // Loop through all pointers to find the block
    do {
        File_s *new_node = (File_s *)(&current->data);

        if (strcmp(name, new_node->name) == 0) {
            found = true;
            break;
        }
        previous = current;
        current = current->next;
    } while(current);

    // Panic if we did not find it
    if (found == false) {
        _kpanic("Data block not found");
    } else {
        cwrites("Found\n");
    }

    /*list_s *temp = node; // Set the temp variable
    temp->data = NULL; // Set the data in temp to NULL
    temp->next = free_block; // Set temps next variable to the head of the free list (now the new head)
    fs->free_blocks = temp; // Set the free nodes to the new list
    node->next = used_block->next; // Remove the node from the used blocks list*/
    if (previous != NULL) {
        previous->next = current->next;
    } else {
        fs->used_blocks = current->next;
    }

    current->next = fs->free_blocks;
    fs->free_blocks = current;

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
        _kpanic("No free nodes");
    }
    if (inode == NULL) {
        _kpanic("Inode is null");
    }

    if (index == 0) {
        if (is_direct) {
            delete_data_block_pointer(fs, inode, index, name);
        } else {
            delete_inode_pointer(fs, inode, index, name);
        }
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
        _kpanic("Pointer is a data block, not an inode");
    }

    fs->previous_inode = inode;
    fs->current_inode = (Inode_s *)(inode->direct[0]);
    
    // Panic here. Something is wrong with the current inode
    if (fs->current_inode == NULL) {
        _kpanic("The current inode is null");
    }

    cwrites("Moved into directory!\n");

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

    // This is not printing out the inode name properly
    sprint(str, "Directory info for %s:\n", inode->name);
    cwrites(str);

    // Loop through each one of the possible pointers in the current inode
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        if (i == 0 && !inode->is_direct) {
            node = (Inode_s *)(inode->direct[i]);
            if (node == NULL)  {
                sprint(str, "Directory: %d. NULL\n", (i+1));
            } else {
                sprint(str, "Directory: %d. %s\n", (i+1), node->name);
            }
        } else {
            file = (File_s *)(inode->direct[i]);
            if (file == NULL) {
                sprint(str, "File: %d. NULL\n", (i+1));
            } else {
                sprint(str, "File: %d. %s\n", (i+1), file->name);
            }
        }

        cwrites(str);
    }

    sprint(str, "Total number of pointers active: %d\n", inode->num_of_pointers);
    cwrites(str);
}

/**
 * @brief Print out the information of the file system such as the number of nodes in used, or freed.
 * 
 * @param fs - the file system structure
 */
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
}