/**
** @file syscalls.h
**
** @author CSCI-452 class of 20225
**
** System call declarations
*/

#ifndef SYSCALLS_H_
#define SYSCALLS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#include "common.h"

// system call codes
//
// these are used in the user-level C library stub functions,
// and are defined here as CPP macros instead of as an enum
// so that they can be used from assembly
#define SYS_exit        0
#define SYS_spawn       1
#define SYS_read        2
#define SYS_write       3
#define SYS_sleep       4
#define SYS_kill        5
#define SYS_waitpid     6
#define SYS_ushell      7

// UPDATE THIS DEFINITION IF MORE SYSCALLS ARE ADDED!
#define N_SYSCALLS      8

// dummy system call code for testing our ISR
#define SYS_bogus       0xbad

// interrupt vector entry for system calls
#define INT_VEC_SYSCALL 0x80

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

#include "queues.h"

/*
** Types
*/

/*
** Globals
*/

// queue of sleeping processes
extern Queue _sleepQueue;

/*
** Prototypes
*/

/**
** Name:  _sys_init
**
** Syscall module initialization routine
*/
void _sys_init( void );

#endif
/* SP_ASM_SRC */

#endif
