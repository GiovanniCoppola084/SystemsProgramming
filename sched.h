/**
** @file sched.h
**
** @author CSCI-452 class of 20225
**
** Scheduler module declarations
*/

#ifndef SCHED_H_
#define SCHED_H_

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

#include "queues.h"

// standard process quantum
#define QUANTUM_DEFAULT     3

/*
** Types
*/

/*
** Globals
*/

// the ready queue:  a MLQ with one level per priority value
extern Queue _readyQueue[N_PRIOS];

// number of processes in each level of the ready queue
extern uint32_t _readyLength[N_PRIOS];

// the current user process
extern Pcb *_current;

/*
** Prototypes
*/

/**
** _sched_init() - initialize the scheduler module
**
** Allocates the ready queues and resets the "current process" pointer
**
** Dependencies:
**    Cannot be called before queues are initialized
**    Must be called before any process scheduling can be done
*/
void _sched_init( void );

/**
** _schedule() - add a process to the ready queue
**
** Enques the supplied process according to its priority value
**
** @param pcb   The process to be scheduled
*/
void _schedule( Pcb *pcb );

/**
** _dispatch() - select a new "current" process
**
** Selects the highest-priority process available
*/
void _dispatch( void );

#endif
/* SP_ASM_SRC */

#endif
