/*
** SCCS ID: @(#)file_template.h	3/23/2023
**
** File:    file_template.h
**
** Authors: Gino Coppola
**
** Description: The prototypes for functions to set up a basic file.
** Base of Unix V7 or Microsoft FAT
** https://www.cs.rit.edu/~wrc/courses/common/notes/csci452/
** https://learn.microsoft.com/en-us/troubleshoot/windows-client/backup-and-storage/fat-hpfs-and-ntfs-file-systems
** https://github.com/v7unix/v7unix
** https://www.tuhs.org/cgi-bin/utree.pl?file=V7
** https://github.com/darshank15/Inode-based-file-system/blob/master/inode.h
*/

#ifndef _FILE_TEMPLATE_H_
#define _FILE_TEMPLATE_H_

#include "cio.h"
#include "users.h"
#include "ulib.h"

/**
* Structure for a file. It will contain a name and contents.
*
* @param filename - The start of a file name (first 14 characters)
* @param node_entry - The last 2 bytes of the file name to signify what position in the directory it is
* @param contents - 47 bytes the user is limited to using. If the user goes over, it will be truncated
*/
typedef struct file_s {
    unsigned char filename[14]; // Beginning of name
    unsigned char node_entry[2]; // End of name where the entry specifies which numebr in the directory it is
    // 'r' - represent read-only
    // 'w' - read and write
    unsigned char attributes; // Two digit hex number that will represent the permmissions
    unsigned char contents[47]; // Truncate bytes if go over 47, so the struct caps on 64 bytes total
} File_s;

/**
* Structure for a directory. It will contain the directory name, number of contents in the directory, and 
*   a list of pointers for the directories and files in the current one.
*
* @param directory_name - 16 bytes for the directory name
* @param number_of_contents - the number of contents in the current directory (including files and subdirectories)
* @param files_and_or_directories - the pointer array for the files and subdirectories in the current directory
*/
typedef struct directory_s {
    unsigned char directory_name[15];
    unsigned char type; // 'f' means file and 'd' means directory
    uint32_t number_of_contents;
    void* files_and_or_directories[20]; // Hard cap so if they go over 20 files/directories, blow up
} Directory_s;

Directory_s working;
File_s file = (File_s) working.file_and_or_directories[1];

/**
* Allocate a directory block. This will be a slice.
*/
Directory_s allocate_directory (void);
/**
* Allocate a file block. This will be the size of a file. (48 bytes right now)
*/
File_s allocate_file (void);

/**
* Create file. Set command line args for permission and name
*/
File_s create_file (uint32_t working_directory, uint16_t node_entry);

/**
* Create directory. Do similar to file with command line args
*/
Directory_s create_directory ();

/**
* Read a file. Return a byte array the file contains
*/
char[] read_file (struct File_s);
/**
* Write to a file. This will prompt the user on the terminal to enter 32 bytes at most
*/
void write_file (struct File_s, char[] input);

/**
* Free a file when it is done being used.
*/
void free_file_s (struct File_s);
/**
* Free a directory when it is done being used
*/
void free_directory_s (struct Directory_s);

#endif
