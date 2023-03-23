/**
** @file stacks.h
**
** @author CSCI-452 class of 20225
**
** Stack module declarations
*/

#ifndef STACKS_H_
#define STACKS_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

#include "common.h"

#include "kmem.h"

// stack size, in bytes and words
//
// for simplicity, our stack is a multiple of the page size (4KB)

// four pages (16KB) per stack
#define STACK_PAGES      4

#define SZ_STACK        (SZ_PAGE * STACK_PAGES)
#define STACK_WORDS     (SZ_STACK / sizeof(uint32_t))

/*
** Types
*/

// the stack
//
// somewhat anticlimactic....

typedef uint32_t Stack[STACK_WORDS];

/*
** Globals
*/

/*
** Prototypes
*/

/*
** Module initialization
*/

/**
** _stk_init() - initialize the stack module
**
** Allocates an initial set of stacks, does whatever else is
** needed to make it possible to create processes
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before any process creation can be done
*/
void _stk_init( void );

/*
** Stack manipulation
*/

/**
** _stk_alloc() - allocate a stack
**
** @return pointer to a "clean" stack, or NULL
*/
Stack *_stk_alloc( void );

/**
** _stk_free() - free a stack
**
** @param data   The stack to be returned to the free list
*/
void _stk_free( void *data );

// need Context now
#include "processes.h"

/**
** _stk_setup - set up the stack for a new process
**
** @param stk   The stack to be set up
** @param len   Length of the argument buffer
** @param arg   Argument buffer
**
** @return A pointer to the Context on the stack, or NULL
*/
Context *_stk_setup( Stack *stk, uint32_t len, void *buffer );

/*
** Debugging/tracing routines
*/

/**
** _stk_dump(msg,stk,lim)
**
** Dumps the contents of this stack to the console.  Assumes the
** stack is a multiple of four words in length.
**
** @param msg   An optional message to print before the dump
** @param stk   The stack to dump out
** @param lim   Limit on the number of words to dump (0 for all)
*/
void _stk_dump( const char *msg, register Stack *stk, uint32_t lim );

#endif
/* SP_ASM_SRC */

#endif
