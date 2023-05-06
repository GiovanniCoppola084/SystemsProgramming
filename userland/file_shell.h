// 
// File: file_shell.h 
// Description: Shell commands to operate the file system
//
// @author Gino Coppola
//
// Description: Function prototypes for the functions used by the shell
// 
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

#include "./file_template.h"

#define MAX_LINE_LENGTH 80

/**
 * @brief This function will read in the line entered by the user one character at a time.
 *        It will also print the prompt before entering the user information
 * 
 * @param buffer - the buffer that the data will be put in
 * @param length - the length of the input data
 * @param prompt - the prompt to print before getting the user information
 */
void read_line (char *buffer, uint32_t length, char *prompt);

/**
 * @brief This function will break the system if the file system has not been initialized before
 *        doing an operation on it
 * 
 */
void is_fs_initialized(void);

/**
 * @brief This function will break the system if the first inode has not been initialized before
 *        doing an operation on it
 * 
 */
void is_inode(void);

/**
 * @brief This function will print all of the options of the menu to the screen
 * 
 */
void print_menu(void);