/*
** SCCS ID: @(#)file_template.h	3/23/2023
**
** File:    file_template.h
**
** Authors: Gino
**
** Description: The prototypes for functions to set up a basic file.
** Base of Unix V7 or Microsoft FAT
** https://www.cs.rit.edu/~wrc/courses/common/notes/csci452/
** https://learn.microsoft.com/en-us/troubleshoot/windows-client/backup-and-storage/fat-hpfs-and-ntfs-file-systems
** https://github.com/v7unix/v7unix
** https://www.tuhs.org/cgi-bin/utree.pl?file=V7
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
* @param contents - 32 bytes the user is limited to using. If the user goes over, it will be truncated
*/
typedef struct file_s {
    unsigned char filename[14]; // Beginning of name
    unsigned char node_entry[2]; // End of name where the entry specifies which numebr in the directory it is
    unsigned char contents[32]; // Truncate bytes if go over 32
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
    unsigned char directory_name[16];
    uint32_t number_of_contents;
    unsigned int files_and_or_directories[20]; // Hard cap so if they go over 20 files/directories, blow up
} Directory_s;

/**
* Allocate a directory block. This will be a slice.
*/
Directory_s allocate_directory (void);
/**
* Allocate a file block. This will be the size of a file. (48 bytes right now)
*/
File_s allocate_file (void);

/**
* Free a file when it is done being used.
*/
void free_file_s (struct File_s);
/**
* Free a directory when it is done being used
*/
void free_directory_s (struct Directory_s);

#endif
