/**
** @file ulib.h
**
** @author Numerous CSCI-452 classes
**
** Declarations for user-level library functions
**
** This module implements a simple collection of support functions
** similar to the standard C library.
*/

#ifndef ULIB_H_
#define ULIB_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
**********************************************
** SYSTEM CALLS
**********************************************
*/

/**
** exit - terminate the calling process
**
** usage:   exit(status);
**
** @param status   Termination status of this process
**
** @return Does not return
*/
void exit( int32_t status );

/**
** spawn - create a new process
**
** usage:   pid = spawn(entry,args)
**
** @param entry The entry point of the new code
** @param prio  The process priority for the new process
** @param len   Length of argument buffer
** @param arg   Argument buffer
**
** @return the child's PID, or a negative value on error
*/
int spawn( void *entry, prio_t prio, uint32_t len, const void *arg  );

/**
** read - read into a buffer from a stream
**
** usage:   n = read(channel,buf,length)
**
** @param chan   I/O stream to read from
** @param buf    Buffer to read into
** @param length Maximum capacity of the buffer
**
** @returns  The count of bytes transferred, or an error code
*/
int read( int chan, void *buffer, uint32_t length );

/**
** write - write from a buffer to a stream
**
** usage:   n = write(channel,buf,length)
**
** @param chan   I/O stream to write to
** @param buf    Buffer to write from
** @param length Maximum capacity of the buffer
**
** @returns  The count of bytes transferred, or an error code
*/
int write( int chan, const void *buffer, uint32_t length );

/**
** sleep - put the current process to sleep for some length of time
**
** usage:   sleep(n);
**
** @param n Desired sleep time (in what units?), or 0 to yield the CPU
*/
void sleep( uint32_t n );

/**
** kill - terminate a process with extreme prejudice
**
** usage:   n = kill(pid);
**
** @param pid The PID of the desired victim, or 0 for this process
**
** @returns 0 on success, -1 on error
*/
int kill( pid_t pid );

/**
** waitpid - wait for a child process to terminate
**
** usage:   pid = waitpid(pid,&status);
**
** @param pid    PID of the desired child, or 0 for any child
** @param status Pointer to int32_t into which the child's status is placed,
**               or NULL
**
** @returns The PID of the terminated child, or an error code
**
** If there are no children in the system, returns an error code (*status
** is unchanged).
**
** If there are one or more children in the system and at least one has
** terminated but hasn't yet been cleaned up, cleans up that process and
** returns its information; otherwise, blocks until a child terminates.
*/
int waitpid( pid_t pid, int32_t *status );

/**
** ushell - register this process as the "user shell" process
**
** usage:   n = ushell();
**
** @return zero if the registration works, else the PID of the process
** currently registered as the user shell
*/
int ushell( void );

/**
** bogus - a bogus system call, for testing our syscall ISR
**
** usage:   bogus();
**
** @return Does not return
*/
void bogus( void );

/*
**********************************************
** CONVENIENT "SHORTHAND" VERSIONS OF SYSCALLS
**********************************************
*/

/** 
** wait - wait for any child to exit
**
** usage:   pid = wait(&status)
**
** Calls waitpid(0,status)
**
** @param status Pointer to int32_t into which the child's status is placed,
**               or NULL
**
** @returns The PID of the terminated child, or an error code
*/
int wait( int32_t *status );

/**
** cwritech(ch) - write a single character to the console
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int cwritech( char ch );

/**
** cwrites(str) - write a NUL-terminated string to the console
**
** @param str The string to write
**
*/
int cwrites( const char *str );

/**
** cwrite(buf,leng) - write a sized buffer to the console
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @returns The return value from calling write()
*/
int cwrite( const char *buf, uint32_t leng );

/**
** swritech(ch) - write a single character to the SIO
**
** @param ch The character to write
**
** @returns The return value from calling write()
*/
int swritech( char ch );

/**
** swrites(str) - write a NUL-terminated string to the SIO
**
** @param str The string to write
**
** @returns The return value from calling write()
*/
int swrites( const char *str );

/**
** swrite(buf,leng) - write a sized buffer to the SIO
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @returns The return value from calling write()
*/
int swrite( const char *buf, uint32_t leng );

/*
**********************************************
** STRING MANIPULATION FUNCTIONS
**********************************************
*/

/**
** str2int(str,base) - convert a string to a number in the specified base
**
** @param str   The string to examine
** @param base  The radix to use in the conversion
**
** @return The converted integer
*/
int str2int( register const char *str, register int base );

/**
** strlen(str) - return length of a NUL-terminated string
**
** @param str The string to examine
**
** @return The length of the string, or 0
*/
uint32_t strlen( const char *str );

/**
** strcpy(dst,src) - copy a NUL-terminated string
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the copied string
*/
char *strcpy( register char *dst, register const char *src );

/**
** strcat(dst,src) - append one string to another
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the resulting string
*/
char *strcat( register char *dst, register const char *src );

/**
** strcmp(s1,s2) - compare two NUL-terminated strings
**
** @param s1 The first source string
** @param s2 The second source string
**
** @return negative if s1 < s2, zero if equal, and positive if s1 > s2
*/
int strcmp( register const char *s1, register const char *s2 );

/**
** pad(dst,extra,padchar) - generate a padding string
**
** @param dst     Pointer to where the padding should begin
** @param extra   How many padding bytes to add
** @param padchar What character to pad with
**
** @return Pointer to the first byte after the padding
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *pad( char *dst, int extra, int padchar );

/**
** padstr(dst,str,len,width,leftadjust,padchar - add padding characters
**                                               to a string
**
** @param dst        The destination buffer
** @param str        The string to be padded
** @param len        The string length, or -1
** @param width      The desired final length of the string
** @param leftadjust Should the string be left-justified?
** @param padchar    What character to pad with
**
** @return Pointer to the first byte after the padded string
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *padstr( char *dst, char *str, int len, int width,
                   int leftadjust, int padchar );

/**
** sprint(dst,fmt,...) - formatted output into a string buffer
**
** @param dst The string buffer
** @param fmt Format string
**
** The format string parameter is followed by zero or more additional
** parameters which are interpreted according to the format string.
**
** NOTE:  assumes the buffer is large enough to hold the result string
**
** NOTE:  relies heavily on the x86 parameter passing convention
** (parameters are pushed onto the stack in reverse order as
** 32-bit values).
*/
void sprint( char *dst, char *fmt, ... );

/*
**********************************************
** MISCELLANEOUS USEFUL SUPPORT FUNCTIONS
**********************************************
*/

/**
** fallbackExit() - dummy "startup" function
**
** calls exit(FAKE_EXIT) - serves as the "return to" code for
** main() functions, in case they don't call exit() themselves
*/
void fallbackExit( void );

/**
** cvt_dec(buf,value)
**
** convert a 32-bit signed value into a NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_dec( char *buf, int32_t value );

/**
** cvt_hex(buf,value)
**
** convert a 32-bit unsigned value into an (up to) 8-character
** NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_hex( char *buf, uint32_t value );

/**
** cvt_oct(buf,value)
**
** convert a 32-bit unsigned value into an (up to) 11-character
** NUL-terminated character string
**
** @param buf   Destination buffer
** @param value Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int cvt_oct( char *buf, uint32_t value );

/**
** parseArgString(argc,args,n,argv)
**
** parse a command-line argument string into an argument vector
**
** @param argc    Count of arguments expected in the string
** @param args    The command-line argument string
** @param sep     The character separating arguments within 'args'
** @param n       Length of the argv array
** @param argv    The argv array
**
** @returns The number of argument strings put into the argv array,
** or -1 on error
**
** Takes the argc and args parameters to the process' main() function
** along with an array of char *; fills in the array with pointers to
** the beginnings of the argument strings, followed by a NULL pointer.
** Replaces the separator character with NUL characters.  Only converts
** the first n-1 entries.
*/
int parseArgString( int argc, char *args, char sep, int n, char *argv[] );

#endif
/* SP_ASM_SRC */

#endif
