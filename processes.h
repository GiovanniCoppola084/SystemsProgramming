/**
** @file processes.h
**
** @author CSCI-452 class of 20225
**
** Process module declarations
*/

#ifndef PROCESSES_H_
#define PROCESSES_H_

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
** Process context structure
**
** NOTE:  the order of data members here depends on the
** register save code in isr_stubs.S!!!!
**
** This will be at the top of the user stack when we enter
** an ISR.  In the case of a system call, it will be followed
** by the return address and the system call parameters.
*/

typedef struct context_s {
    uint32_t ss;        // pushed by isr_save
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t vector;
    uint32_t code;
    uint32_t eip;       // pushed by the hardware
    uint32_t cs;
    uint32_t eflags;
} Context;

#define SZ_CONTEXT  sizeof(Context)

// process states and priorities may need to be moved
// to common.h if they are going to be visible to userland

// Process states
enum state_e {
    // ordinary states
    Unused = 0, New,
    // "active" states
    Ready, Running, Sleeping, Waiting, Blocked,
    // "dead" states
    Zombie, Killed,
    // sentinel - value equals the number of states
    N_STATES
};

typedef uint8_t state_t;

// Process priorities are defined in common.h

/*
** Sigh.  We need to know what a Stack looks like, but the Stack
** module needs to know what a Context looks like, so we can't
** do the include of stacks.h until here.
*/
#include "stacks.h"

/*
** the process control block
**
** fields are ordered by size to avoid padding
**
** ideally, its size should divide evenly into 1024 bytes;
** currently, 32 bytes
*/

typedef struct pcb_s {
    // four-byte values

    // Start with these eight bytes, for easy access in assembly
    Context *context;   // pointer to context save area on stack
    Stack *stack;       // pointer to process stack

    int32_t status;     // termination status, for parent's use
    uint32_t wakeup;    // wakeup time for this process when sleeping
    uint32_t working_directory; // working the directoy the user is in when they boot up the file system

    // two-byte values

    pid_t pid;          // unique PID for this process
    pid_t ppid;         // PID of the parent

    // one-byte values
    state_t state;      // current state (see common.h)
    prio_t priority;    // process priority (MLQ queue level)

    uint8_t quantum;    // remaining quantum for this process

    // filler, to round us up to 32 bytes
    // adjust this as fields are added/removed/changed
    uint8_t filler[5];
    //uint8_t filler[9];

} Pcb;

#define SZ_PCB  sizeof(Pcb)

/*
** Globals
*/

// next available PID
extern pid_t _nextPid;

// active process count
extern uint32_t _nProcs;

// table of active processes
extern Pcb *_activeProcs[ MAX_PROCS ];

// pointer to the PCB for init()
extern Pcb *_initPCB;

// pointer to the "user shell" (if there is one)
extern Pcb *_userShell;

/*
** Prototypes
*/

/*
** Module initialization
*/

/**
** _pcb_init() - initialize the process module
**
** Allocates an initial set of PCBs, does whatever else is
** needed to make it possible to create processes
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before any process creation can be done
*/
void _pcb_init( void );

/*
** PCB manipulation
*/

/**
** _pcb_alloc() - allocate a PCB
**
** @return A pointer to a "clean" PCB, or NULL
*/
Pcb *_pcb_alloc( void );

/**
** _pcb_free() - free a PCB
**
** @param data   The PCB to be returned to the free list
*/
void _pcb_free( void *data );

/**
** _pcb_find() - locate a PCB by PID
**
** @param pid   The PID we want to locate
**
** @return A pointer to the desired PCB, or NULL
*/
Pcb *_pcb_find( pid_t pid );

/**
** _pcb_cleanup() - reclaim a process' data structures
**
** @param pcb   The PCB to reclaim
*/
void _pcb_cleanup( Pcb *pcb );

/**
** _performExit - do the real work for exit() and some kill() calls
**
** Also called from the scheduler and the dispatcher.
**
** @param victim  Pointer to the PCB for the exiting process
*/
void _performExit( Pcb *victim );

/**
** _pcount - count the number of active processes in each state
**
** @param counts  Array into which the counts are placed
**
** @return The number of active processes
*/
int _pcount( register uint32_t *counts );

/*
** Debugging/tracing routines
*/

/**
** _pcb_dump(msg,pcb)
**
** Dumps the contents of this PCB to the console
**
** @param msg   An optional message to print before the dump
** @param p     The PCB to dump out
*/
void _pcb_dump( const char *msg, register Pcb *p );

/**
** _ctx_dump(msg,context)
**
** Dumps the contents of this process context to the console
**
** @param msg   An optional message to print before the dump
** @param c     The context to dump out
*/
void _ctx_dump( const char *msg, register Context *c );

/**
** _ctx_dump_all(msg)
**
** dump the process context for all active processes
**
** @param msg  Optional message to print
*/
void _ctx_dump_all( const char *msg );

/**
** _ptable_dump(msg,all)
**
** dump the contents of the "active processes" table
**
** @param msg  Optional message to print
** @param all  Dump all or only part of the relevant data
*/
void _ptable_dump( const char *msg, bool_t all );

#endif
/* SP_ASM_SRC */

#endif
