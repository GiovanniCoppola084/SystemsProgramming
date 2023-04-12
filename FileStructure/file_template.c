#include "file_template.h"
#include "kmem.h"

/**
* Create the file system by allocating a slab for the entire thing (since we limit to 128 inodes)
*/
uint16_t create_file_system (FileSystem_s *fs) {
    fs = (FileSystem_s*) _km_page_alloc(1);
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
    
}

/**
* Read what is in the data block to the file
*/
void inode_write (FileSystem_s *fs, uint32_t inode_number, char *data_block, uint32_t length) {

}