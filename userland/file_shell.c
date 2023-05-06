#include "./file_template.h"

#define MAX_LINE_LENGTH 80

FileSystem_s *fs;

// Add parameter to print a prompt, user will enter empty string if no prompt
void read_line (char *buffer, uint32_t length) {
    char *end = buffer + length - 1;

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

void is_fs_initialized() {
    if (fs == NULL) {
        _kpanic("File system not yet initalized");
    }
}

void is_inode() {
    if (fs->current_inode == NULL) {
        _kpanic("No inode exists");
    }
}

void print_menu() {
    swrites("COMMANDS:\r\n");
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
            // Read one character from serial
            read(CHAN_SIO, &buffer, 1);
            swritech(buffer);
            swrites("\r\n");

            char name[MAX_NAME_LENGTH];
            char data_block[SIZE_OF_DATA_BLOCK_DEC];
            char index_str[3];
            int index;

            switch(buffer) {
                case 'i':
                    swrites("Initializing file system!\r\n");
                    fs = file_system_init();
                    break;
                case 'c':
                    is_fs_initialized();
                    swrites("Enter a name for the inode: ");
                    read_line(name, MAX_LINE_LENGTH);
                    swrites("\r\n");
                    fs->current_inode = create_inode(fs, fs->current_inode, 0, false, name);
                    break;
                case 'd':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter a name for the data block: ");
                    read_line(name, MAX_NAME_LENGTH);
                    swrites("\r\n");
                    swrites("Enter data for the data block: ");
                    read_line(data_block, SIZE_OF_DATA_BLOCK_DEC);
                    swrites("\r\n");
                    swrites("Enter index of the data block: ");
                    read_line(index_str, 3);
                    swrites("\r\n");
                    index = str2int(index_str, 10);
                    create_data_block(fs, fs->current_inode, true, name, data_block, (index-1));
                    break;
                case 'r':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter the number of the data block to read: ");
                    read_line(index_str, 3);
                    swrites("\r\n");
                    index = str2int(index_str, 10);
                    inode_read(fs, fs->current_inode, (index-1));
                    break;
                case 'w':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter the index of the data block to write to: ");
                    read_line(index_str, 3);
                    index = str2int(index_str, 10);
                    swrites("\r\n");
                    swrites("Enter the new data: ");
                    read_line(data_block, SIZE_OF_DATA_BLOCK_DEC);
                    swrites("\r\n");
                    inode_write(fs, fs->current_inode, (index-1), data_block);
                    break;
                case 'm':
                    // Don't move into directory for now
                    break;
                case 'o':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter the index of the data block to overwrite: ");
                    read_line(index_str, 3);
                    index = str2int(index_str, 10);
                    swrites("\r\n");
                    inode_delete_data(fs, fs->current_inode, (index-1));
                    break;
                case 'p':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter the index of the inode to delete: ");
                    read_line(index_str, 3);
                    index = str2int(index_str, 10);
                    swrites("\r\n");
                    swrites("Enter the name of the inode: ");
                    read_line(name, MAX_NAME_LENGTH);
                    swrites("\r\n");
                    delete_inode_pointer(fs, fs->current_inode, (index-1), name);
                    break;
                case 'b':
                    is_fs_initialized();
                    is_inode();
                    swrites("Enter the index of the data block to delete: ");
                    read_line(index_str, 3);
                    index = str2int(index_str, 10);
                    swrites("\r\n");
                    swrites("Enter the name of the data block: ");
                    read_line(name, MAX_NAME_LENGTH);
                    swrites("\r\n");
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
            }
        }
    }
}