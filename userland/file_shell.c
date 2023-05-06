#include "file_shell.h"

FileSystem_s *fs;

/**
 * @brief This function will read in the line entered by the user one character at a time.
 *        It will also print the prompt before entering the user information
 * 
 * @param buffer - the buffer that the data will be put in
 * @param length - the length of the input data
 * @param prompt - the prompt to print before getting the user information
 */
void read_line (char *buffer, uint32_t length, char *prompt) {
    char *end = buffer + length - 1;

    // Not sure if this is allowed
    swrites(prompt);

    while (1) {
        if (buffer == end) {
            *buffer = '\0'; 
            break;
        }

        char one_time;
        read(CHAN_SIO, &one_time, 1);
        if (one_time == '\n') {
            *buffer = '\0';
            break;
        } else {
            *buffer++ = one_time;
            swritech(one_time);
        }
    }

    swrites("\r\n");
}

/**
 * @brief This function will break the system if the file system has not been initialized before
 *        doing an operation on it
 * 
 */
void is_fs_initialized(void) {
    if (fs == NULL) {
        _kpanic("File system not yet initalized");
    }
}

/**
 * @brief This function will break the system if the first inode has not been initialized before
 *        doing an operation on it
 * 
 */
void is_inode(void) {
    if (fs->current_inode == NULL) {
        _kpanic("No inode exists");
    }
}

/**
 * @brief This function will print all of the options of the menu to the screen
 * 
 */
void print_menu(void) {
    swrites("Commands:\r\n");
    swrites("i - initialize file system\r\n");
    swrites("c - create inode\r\n");
    swrites("d - create data block\r\n");
    swrites("r - read data block\r\n");
    swrites("w - write to data block\r\n");
    swrites("m - move into directory\r\n");
    swrites("o - overwrite data block\r\n");
    swrites("p - delete inode\r\n");
    swrites("b - delete data block\r\n");
    swrites("y - print directory\r\n");
    swrites("z - print file system information\r\n");
    swrites("x - exit\r\n");
    swrites("h - print menu\r\n");
}
 
USERMAIN (file_shell) {
    while (1) {
        print_menu();

        char buffer;

        while(1) {
            // Read one character from serial. This will be the command character
            read(CHAN_SIO, &buffer, 1);
            swritech(buffer);
            swrites("\r\n");

            // Simple variables that are used in many of the cases
            char name[MAX_NAME_LENGTH];
            char data_block[SIZE_OF_DATA_BLOCK_DEC];
            char index_str[3];
            int index;

            // Do an operation based on which one the user picked
            // Commands are described more closely in the print menu function
            switch(buffer) {
                case 'i':
                    swrites("Initializing file system!\r\n");
                    fs = file_system_init();
                    break;
                case 'c':
                    is_fs_initialized();
                    read_line(name, MAX_LINE_LENGTH, "Enter a name for the inode: ");
                    create_inode(fs, fs->current_inode, name);
                    break;
                case 'd':
                    is_fs_initialized();
                    is_inode();
                    read_line(name, MAX_NAME_LENGTH, "Enter a name for the data block: ");
                    read_line(data_block, SIZE_OF_DATA_BLOCK_DEC, "Enter data for the data block: ");
                    read_line(index_str, 3, "Enter index of the data block: ");
                    index = str2int(index_str, 10);
                    create_data_block(fs, fs->current_inode, true, name, data_block, (index-1));
                    break;
                case 'r':
                    is_fs_initialized();
                    is_inode();
                    read_line(index_str, 3, "Enter the number of the data block to read: ");
                    index = str2int(index_str, 10);
                    inode_read(fs, fs->current_inode, (index-1));
                    break;
                case 'w':
                    is_fs_initialized();
                    is_inode();
                    read_line(index_str, 3, "Enter the index of the data block to write to: ");
                    index = str2int(index_str, 10);
                    read_line(data_block, SIZE_OF_DATA_BLOCK_DEC, "Enter the new data: ");
                    inode_write(fs, fs->current_inode, (index-1), data_block);
                    break;
                case 'm':
                    move_in_directory(fs, fs->current_inode);
                    break;
                case 'o':
                    is_fs_initialized();
                    is_inode();
                    read_line(index_str, 3, "Enter the index of the data block to overwrite: ");
                    index = str2int(index_str, 10);
                    inode_delete_data(fs, fs->current_inode, (index-1));
                    break;
                case 'p':
                    is_fs_initialized();
                    is_inode();
                    read_line(index_str, 3, "Enter the index of the inode to delete: ");
                    index = str2int(index_str, 10);
                    read_line(name, MAX_NAME_LENGTH, "Enter the name of the inode: ");
                    delete_inode_pointer(fs, fs->current_inode, (index-1), name);
                    break;
                case 'b':
                    is_fs_initialized();
                    is_inode();
                    read_line(index_str, 3, "Enter the index of the data block to delete: ");
                    index = str2int(index_str, 10);
                    read_line(name, MAX_NAME_LENGTH, "Enter the name of the data block: ");
                    delete_inode_pointer(fs, fs->current_inode, (index-1), name);
                    break;
                case 'y':
                    is_fs_initialized();
                    is_inode();
                    print_directory(fs, fs->current_inode);
                    break;
                case 'z':
                    is_fs_initialized();
                    print_file_system_info(fs);
                    break;
                case 'x':
                    exit(1);
                    break;
                case 'h':
                    print_menu();
                    break;
                default:
                    print_menu();
                    break;
            }
        }
    }
}