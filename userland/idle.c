#ifndef IDLE_H_
#define IDLE_H_

#include "users.h"
#include "ulib.h"
#include "FileStructure/file_template.h"

/**
** Idle process:  write, getpid, gettime, exit
**
** Reports itself, then loops forever delaying and printing a character.
**
** Invoked as:  idle
*/

USERMAIN( idle ) {
    char ch = '.';

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
    Inode_s *inode = create_inode(fs, "first_folder", NULL, 0, false, NULL);
    assert(inode != NULL);

    /* Print that the inode was created successfully */
    char str[80];
    sprint(str, "Inode created!\n");
    cwrites(str);
    DELAY(LONG * 10);

    /* Set the current working directory in the file system */
    fs->current_inode = inode;

    /* Create a full data block */
    char block[512];
    for (int i = 0; i < 512; i++) {
        block[i] = 'a';
    }

    /* Create the first data block in the inode */
    create_data_block(fs, inode, false, "first_file", block, 1);
    sprint(str, "Data block created!\n");
    cwrites(str);
    DELAY(LONG * 10);
    
    /* Read the inode data that we just input */
    inode_read(fs, inode, 1);
    DELAY(LONG * 10);

    /* Delete the data that was in the data block */
    inode_delete_data(fs, inode, 1);
    sprint(str, "Block data deleted!\n");
    cwrites(str);
    DELAY(LONG * 10);

    /* Reprint the data that should be all spaces */
    inode_read(fs, inode, 1);
    DELAY(LONG * 10);

    /* Print the current working directory */
    print_directory(fs, inode);
    DELAY(LONG * 10);

    /* Create the next pointer to an inode in the working directory */
    inode->direct[0] = create_inode(fs, "sec_dir", NULL, 0, false, NULL);
    sprint(str, "Inode created!\n");
    cwrites(str);
    DELAY(LONG * 10);

    /* Print the current working directory */
    print_directory(fs, (Inode_s *)(inode->direct[0]));
    DELAY(LONG * 10);

    /* Move directories to the new one that we made */
    inode = move_in_directory(fs, inode);
    print_directory(fs, inode);
    DELAY(LONG * 10);

    /* Create one more inode */
    inode->direct[0] = create_inode(fs, "third_dir", NULL, 0, false, NULL);
    print_directory(fs, inode);
    DELAY(LONG * 10);

    /* Delete the inode pointer that we just created */
    delete_pointer_in_inode(fs, inode, 0, false);
    print_directory(fs, inode);
    DELAY(LONG * 10);

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
