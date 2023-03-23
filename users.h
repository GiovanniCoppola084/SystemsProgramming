/**
** @file users.h
**
** @author CSCI-452 class of 20225
**
** "Userland" configuration information
*/

#ifndef USERS_H_
#define USERS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

// Maximum number of command-line arguments that can be passed
// to a process.  Not a true limit; used to dimension the argv[]
// arrays in the various user functions, for convenience.

#define MAX_ARGS        7

// delay loop counts

#define DELAY_LONG      100000000
#define DELAY_MED       4500000
#define DELAY_SHORT     2500000

#define DELAY_STD       DELAY_SHORT

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

// convenience macros

// a delay loop

#define DELAY(n)    do { \
        for(int _dlc = 0; _dlc < (DELAY_##n); ++_dlc) continue; \
    } while(0)

// Entry point definition

#define USERMAIN(f)  int32_t f( uint32_t arglen, void *args )

/*
** Command-line argument processing is done the same way for all
** user processes; this macro expands into the code to do it.
**
** ASSUMES that the arg buffer parameter is really a C string
** pointer, and not an arbitrary array of bytes.  If the argument
** is actually binary data (e.g., a struct, or binary numbers),
** DO NOT USE THIS MACRO.
*
** Usage:
**    ARG_PROC( #exp, argparam, argvlen, countvar, my_name );
**
**      #_exp     - how many arguments we expect (0 == variable)
**      argparam  - the arg buffer param (assumed to really be char *)
**      argvlen   - how large to make argv (>= #_expected + 1)
**      countvar  - variable to create that will hold the arg count
**      my_name   - string literal to use as the function's name
**
** Creates argv[argvlen] and countvar; calls parseArgString() to process
** the argument string (assumes the separator character will be '\r');
** assigns the return value to count_var.  Prints a console message if the
** return value from parseArgString() is not equal to the #_expected value
** (if #_expected is non-zero).
**
** We create argv here because this allows us to ensure that all its
** elements contain NULL pointers.  The C standard states, in section
** 6.7.8, that
**
**   "21 If there are fewer initializers in a brace-enclosed list
**       than there are elements or members of an aggregate, or
**       fewer characters in a string literal used to initialize an
**       array of known size than there are elements in the array,
**       the remainder of the aggregate shall be initialized
**       implicitly the same as objects that have static storage
**       duration."
**
** We use an initializer list with a single 0 in it to force argv[0]
** to contain a NULL pointer, and the other entries automatically 
** contain NULL thanks to the C specification. :-)
*/
#define ARG_PROC( n, str, vlen, count, name ) \
    char *argv[ vlen ] = { 0 }; \
    int count = parseArgString( n, (char *) str, '\r', vlen, argv ); \
    if( n != 0 && count != n ) { \
        char _buf[128]; \
        sprint( _buf, "%s: got %d args, expected %d", name, count, n ); \
        cwrites( _buf ); \
    }

/*
** System call matrix
**
** System calls in this system:   exit, spawn, read, write, sleep,
**  kill, waitpid.
**
** There is also a "bogus" system call which attempts to use an invalid
** system call code; this should be caught by the syscall handler and
** the process should be terminated.
**
** These are the system calls which are used in each of the user-level
** main functions.  Some main functions only invoke certain system calls
** when given particular command-line arguments (e.g., main6).
**
** Note that some system calls are nested inside library functions - e.g.,
** cwrite() performs write(), etc.
**
**                        baseline system calls in use
**  fcn   exit  spawn read  write sleep kill  wtpid  bogus
** -----  ----- ----- ----- ----- ----- ----- -----  -----
** main1    X     .     .     X     .     .     .      .
** main2    .     .     .     X     .     .     .      .
** main3    X     .     .     X     X     .     .      .
** main4    X     X     .     X     X     .     .      .
** main5    X     X     .     X     .     .     .      .
** main6    X     X     .     X     X     X     X      .
** .......................................................
** userH    X     X     .     X     X     .     .      .
** userI    X     X     .     X     X     X     X      .
** userJ    X     X     .     X     .     .     .      .
** userP    X     .     .     X     X     .     .      .
** userQ    X     .     .     X     .     .     .      X
** userR    X     .     X     X     X     .     .      .
** userS    X     .     .     X     X     .     .      .
** userW    X     .     .     X     X     .     .      .
** userX    X     .     .     X     .     .     .      .
** userY    X     .     .     X     X     .     .      .
** userZ    X     .     .     X     X     .     .      .
*/

/*
** User process controls.
**
** To spawn a specific user process from the initial process, uncomment
** its entry in this list.
**
** Generally, most of these will exit with a status of 0.  If a process
** returns from its main function when it shouldn't (e.g., if it had
** called exit() but continued to run), it will usually return a status
** of ?.
*/

#define SPAWN_A
#define SPAWN_B
#define SPAWN_C
#define SPAWN_D
#define SPAWN_E
#define SPAWN_F
#define SPAWN_G
#define SPAWN_H
#define SPAWN_I
#define SPAWN_J
#define SPAWN_K
#define SPAWN_L
#define SPAWN_M
#define SPAWN_N
#define SPAWN_P
#define SPAWN_Q
#define SPAWN_R
#define SPAWN_S
#define SPAWN_T
#define SPAWN_U
#define SPAWN_V

//
// There is no userO.  Users W-Z are spawned from other
// processes; they should never be spawned directly by init().
//
// Not all users have 'userX' main functions
//

/*
** We also have a user-level shell that can be spawned.
*/

#ifdef USER_SHELL
#define SPAWN_SHELL
#endif

/*
** Prototypes for externally-visible routines
*/

/**
** init - initial user process
**
** Spawns the other user processes, then loops forever calling wait()
**
** Invoked as:  init
*/
int32_t init( uint32_t arglen, void *args );

/**
** idle - the idle process
**
** Reports itself, then loops forever delaying and printing a character.
**
** Invoked as:  idle
*/
int32_t idle( uint32_t arglen, void *args );

#endif
/* SP_ASM_SRC */

#endif
