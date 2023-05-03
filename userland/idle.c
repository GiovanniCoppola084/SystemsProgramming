#ifndef IDLE_H_
#define IDLE_H_

#include "users.h"
#include "ulib.h"
#include "file_template.h"

/**
** Idle process:  write, getpid, gettime, exit
**
** Reports itself, then loops forever delaying and printing a character.
**
** Invoked as:  idle
*/

USERMAIN( idle ) {
    char ch = '.';
    char str[80];
    uint8_t index_into_inode = 0;

    // ignore the command-line arguments
    (void) arglen;
    (void) args;
    
    // report our presence on the console
    cwrites( "Idle started\n" );

    write( CHAN_SIO, &ch, 1 );

    // idle() should never block - it must always be available
    // for dispatching when we need to pick a new current process

    /* Create the file system and the first inode */
    FileSystem_s *fs = file_system_init();
    sprint(str, "File system location: %08x\n", fs);
    cwrites(str);
    DELAY(LONG * 20);
    print_file_system_info(fs);
    fs->current_inode = create_inode(fs, fs->current_inode, 0, false, "Direc 1");
    // print_directory(fs, fs->current_inode);
    print_file_system_info(fs);
    DELAY(LONG * 20);

    /* Create a full data block */
    char block[SIZE_OF_DATA_BLOCK_DEC];
    for (int i = 0; i < SIZE_OF_DATA_BLOCK_DEC - 1; i++) {
        block[i] = 'a';
    }
    block[SIZE_OF_DATA_BLOCK_DEC - 1] = '\0';

    /* Create the first data block in the inode */
    index_into_inode = 1;
    create_data_block(fs, fs->current_inode, true, "first_file", block, index_into_inode);
    print_directory(fs, fs->current_inode);

    /* Read the inode data that we just input */
    inode_read(fs, fs->current_inode, index_into_inode);
    DELAY(LONG * 20);

    /* Delete the data that was in the data block */
    inode_delete_data(fs, fs->current_inode, index_into_inode);
    /* Reprint the data that should be all spaces */
    inode_read(fs, fs->current_inode, index_into_inode);
    DELAY(LONG * 20);

    // Delete the data pointer from the inode that we just cleared the data from
    Inode_s *inode;
    File_s *file = (File_s *)(fs->current_inode->direct[index_into_inode]);
    delete_inode_pointer(fs, fs->current_inode, index_into_inode, file->name);
    print_file_system_info(fs);
    DELAY(LONG * 20);

    /* Create the next pointer to an inode in the working directory */
    index_into_inode = 0;
    /// Something might be going wrong here in that the node is not being created properly
    inode = (Inode_s *)(fs->current_inode->direct[index_into_inode]);
    inode = create_inode(fs, inode, index_into_inode, false, "Direc 2");

    /* Move directories to the new one that we made */
    move_in_directory(fs, fs->current_inode);
    print_directory(fs, fs->current_inode);
    DELAY(LONG * 20);
    
    /* Create one more inode */
    index_into_inode = 0;
    fs->current_inode->direct[index_into_inode] = create_inode(fs, fs->current_inode->direct[index_into_inode], index_into_inode, false, "Direc 3");
    print_directory(fs, fs->current_inode);
    DELAY(LONG * 20);

    /* Delete the inode pointer that we just created */
    index_into_inode = 0;
    inode = (Inode_s *)(fs->current_inode->direct[index_into_inode]);
    delete_pointer_in_inode(fs, inode, index_into_inode, false, inode->name);
    print_directory(fs, fs->current_inode);
    DELAY(LONG * 20);

    /* Move back a directory */
    move_out_directory(fs);
    print_directory(fs, fs->current_inode);
    DELAY(LONG * 20);

    /* Delete the second directory that we made */
    inode = (Inode_s *)(fs->current_inode->direct[index_into_inode]);
    delete_pointer_in_inode(fs, inode, index_into_inode, false, inode->name);
    print_directory(fs, fs->current_inode);
    DELAY(LONG * 20);

    print_file_system_info(fs);

    for(;;) {
        DELAY(LONG);
        write( CHAN_SIO, &ch, 1 );
    }

    // we should never reach this point!

    cwrites( "+++ IDLE EXITING !?!?!\n" );

    exit( 1 );

    return( 42 );
}

#endif
