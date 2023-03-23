/**
** @file kernel.h
**
** @author Numerous CSCI-452 classes
**
** Miscellaneous OS routines
*/

#ifndef KERNEL_H_
#define KERNEL_H_

#include "common.h"
#include "x86arch.h"

#include "stacks.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

// default contents of EFLAGS register for new processes
#define DEFAULT_EFLAGS  (EFLAGS_MB1 | EFLAGS_IF)

// PID for the init process
#define PID_INIT        1

/*
** Types
*/

/*
** Globals
*/

// character buffers, usable throughout the OS
// not guaranteed to retain their contents across an exception return
extern char b256[256];
extern char b512[512];

// Other system variables (see kernel.c for possible names)

// A separate stack for the OS itself
// (NOTE: this assumes the OS is not reentrant!)
extern Stack *_systemStack;
extern uint32_t *_systemESP;

// table of state name strings
extern const char *_stateStr[][2];
// indices for short and long names
#define L_NAME      0
#define S_NAME      1

// table of priority name strings
extern const char *_prioStr[];

/*
** Prototypes
*/

/**
** _kinit - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/
void _kinit( void );

/**
** _grow_list() - add entries to a free list
**
** Allocates a slice of memory, carves it into entries of the specified
** size, and adds them to the appropriate free list.
**
** @param length   The size of each entry to be added to the list
** @param free     The free() function to be used
**
** @return true if the allocation succeeds, else false
*/
bool_t _grow_list( uint32_t elsize, void (*free)(void *) );

/**
** _kpanic - kernel-level panic routine
**
** usage:  _kpanic( msg )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
**
** @param msg  String containing a relevant message to be printed
*/
void _kpanic( char *msg );

#endif
/* SP_ASM_SRC */

#endif
