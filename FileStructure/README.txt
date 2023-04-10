Make read command public so it can be read by printer code.
Everything else can be private.

Keep track of pointer in PCB since there are 4 bytes at the end of the struct.
Add this at the end of the pcb and then adjust the bytes.

-----------------------------------------------------------------------------------------------------------------------

***********************************************************************************************************************

Directory structure:

Name of directory: 16 bytes
Number of elements in the directory: Inode array size
Contains Inode structure array

***********************************************************************************************************************

Inode structure:

FILE = 0
DIRECTORY = 1

Inode size limit:
File size limit:
Directory size limit:

***********************************************************************************************************************

Contents of an Inode:
Index entry of where it is in the relative structure
0 if something is a directory, 1 if it is a file
Pointer to the file

***********************************************************************************************************************

Contents of a file structure:
File name: 16 bytes in total
File contents: some number of bytes that it can contain in a byte array

***********************************************************************************************************************

-----------------------------------------------------------------------------------------------------------------------


Inode has a ceratin number of data blocks that are directories
Some might have indirect depending on the size
Need to have root dir be pointed to somewhere