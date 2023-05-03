-----------------------------------------------------------------------------------------------------------------------

Unix V7 File System made for csci452
by: Gino Coppola

***********************************************************************************************************************

Contents of an Inode:
Pointer to the file or directory
Number of active pointers
Size (not being used currently. Can be omitted in the future)

***********************************************************************************************************************

Contents of a file structure:
File name: 14 bytes in total
2 bytes for the index in the current inode
File contents: a set number of bytes contained in a byte array

***********************************************************************************************************************

Contents of the file system:
Number of free blocks and inodes
The linked list of memory for the free nodes, used nodes, free blocks, and used blocks
The current inode (working directory) and the previous inode

***********************************************************************************************************************

Contents of the linked list for memory:
Void pointer for the either data block or inode
The next pointer in memory

***********************************************************************************************************************

MEMORY LAYOUT:
0xC0000000 ------>   -----------------------------
                    |        FILE STRUCTURE       |
                    |                             |
                    |                             |
0xC000001D --------> -----------------------------
                    |         INODE LIST          |
                    |                             |
                    |                             |
                    |                             |
                    |                             |
0xC0001F5E --------> -----------------------------
                    |       DATA BLOCK LIST       |
                    |                             |
                    |                             |
                    |                             |
                    |                             |
                    |                             |
                    |                             |
0xC012DF5E ------->  -----------------------------

-----------------------------------------------------------------------------------------------------------------------