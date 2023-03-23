/**
** @file queues.h
**
** @author CSCI-452 class of 20225
**
** Externally-visible parts of the Queue module
*/

#ifndef QUEUES_H_
#define QUEUES_H_

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

/*
** Types
*/

/*
** Our queue structure.  The queue is an opaque type, so the outside
** world only sees it as a "thing".  The basic Queue type is a
** pointer to the internal queue structure.
*/

typedef struct queue_s *Queue;

// payload for queue entries

typedef
    struct payload_s {
        const uint32_t key;
        const uint32_t data;
    } Payload;

/*
** Function pointer types (defined here for convenience)
*/

// _q_forEach() parameter
typedef void (*ApplyFcn)(key_t,const void *);

// _q_find(), _q_removeBy() parameter
typedef bool_t (*LocateFcn)(key_t,const void *,const void *);

// _q_alloc() parameter
typedef int (*OrderFcn)(key_t,key_t);

/*
** Globals
*/

/*
** Prototypes
*/

/**
** Name:  _q_init
**
** Initialize the Queue module
*/
void _q_init( void );

/**
** Name:  _q_free
**
** Free a Queue structure
**
** @param data    The Queue to be freed (as a void*)
*/
void _q_free( void *data );

/**
** Name:  _q_alloc
**
** Allocate a Queue
**
** @param order    Ordering function for non-FIFO queues, or NULL
**
** @return A pointer to the new Queue
*/
Queue _q_alloc( OrderFcn order );

/**
** Name:  _q_length
**
** Return the length of the specified queue
**
** @param que    The Queue to be examined
**
** @return The number of elements currently in the Queue
*/
uint32_t _q_length( Queue que );

/**
** Name:  _q_insert
**
** Perform an insertion (possibly sorted) into a Queue
**
** @param que   the Queue to be manipulated
** @param data  the data value to be inserted
** @param key   the key value associated with the data
**
** @return the number of elements in the Queue after the insertion
*/
uint32_t _q_insert( Queue que, void *data, key_t key );

/**
** Name:  _q_find
**
** Locate an entry in the queue
**
** @param que   the Queue to be examined
** @param data  the data to be located
** @param how   a locater function, or NULL
**
** @return a pointer to a Payload structure, or NULL
*/
Payload *_q_find( Queue que, const void *data, LocateFcn how );

// shorthand for "find the first element"
#define _q_peek(q)  _q_find(q,NULL,NULL)

/**
** Name:  _q_remove
**
** Remove the first node from a Queue
**
** @param que   the Queue to be manipulated
** @param ret   a Payload structure into which the results are placed
**
** @return true if an element was removed; else, false
*/
bool_t _q_remove( Queue que, Payload *ret );

/**
** Name:  _q_removeBy
**
** Remove a node from a Queue using a "locater" function
**
** @param que   the Queue to be manipulated
** @param ret   a Payload structure into which the results are placed
** @param data  the data to be located
** @param how   a locater function
**
** @return true if an element was removed; else, false
*/
bool_t _q_removeBy( Queue que, Payload *ret, const void *data, LocateFcn how );

/**
** Name:  _q_forEach
**
** Iterate through a Queue, applying a processing function to each entry
**
** @param que   the Queue to be manipulated
** @param fcn   the processing function
*/
void _q_forEach( Queue que, ApplyFcn fcn );

/*
** Debugging/tracing routines
*/

/**
** _q_dump(msg,que)
**
** dump the contents of the specified Queue to the console
**
** @param msg  Optional message to print
** @param que  Queue to dump
*/
void _q_dump( const char *msg, Queue que );

#endif
/* SP_ASM_SRC */

#endif
