/**
** @file sched.c
**
** @author CSCI-452 class of 20225
**
** Scheduler module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "sched.h"

#include "processes.h"
#include "queues.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

// the ready queue:  a MLQ with one level per priority value
Queue _readyQueue[N_PRIOS];

// number of processes in each level of the ready queue
uint32_t _readyLength[N_PRIOS];

// the current user process
Pcb *_current;

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
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
void _sched_init( void ) {

    __cio_puts( " Sched" );

    // allocate the ready queues
    for( int i = 0; i < N_PRIOS; ++i ) {
        _readyLength[i] = 0;
        _readyQueue[i] = _q_alloc( NULL );
        // at this point, allocation failure is terminal
        assert( _readyQueue[i] != NULL );
    }

    // reset the "current process" pointer
    _current = NULL;
}

/**
** _schedule() - add a process to the ready queue
**
** Enques the supplied process according to its priority value
**
** @param pcb   The process to be scheduled
*/
void _schedule( Pcb *pcb ) {

#if TRACING_SCHED
    __cio_printf( "** _sched(%08x)", (uint32_t) pcb );
#endif

    // can't enque nothing
    assert1( pcb != NULL );

    // if this process has been terminated, clean it up
    if( pcb->state == Killed ) {
#if TRACING_SCHED
    __cio_puts( " -> KILLED\n" );
#endif
        _performExit( pcb );
        return;
    }

    // bad priority value causes a fault
    assert1( pcb->priority < N_PRIOS );

    // mark the process as ready to execute
    pcb->state = Ready;

#if TRACING_SCHED
    __cio_printf( " adding to RQ[%d] (%d)", pcb->priority,
            _readyLength[pcb->priority] );
#endif
    // add it to the appropriate queue
    _readyLength[pcb->priority] =
        _q_insert( _readyQueue[pcb->priority], (void *)pcb, pcb->pid );
#if TRACING_SCHED
    __cio_printf( " len now %d\n", _readyLength[pcb->priority] );
#endif
}

/**
** _dispatch() - select a new "current" process
**
** Selects the highest-priority process available
*/
void _dispatch( void ) {

#if TRACING_DISPATCH
    __cio_puts( "** _dispatch()\n" );
#endif

    // find a ready queue that has an available process
    Pcb *pcb = NULL;

    do {
        int num;

        for( num = 0; num < N_PRIOS; ++num ) {
            if( _readyLength[num] > 0 ) {
                // found one!
                break;
            }
        }
#if TRACING_DISPATCH
    __cio_printf( "** num/N_PRIOS %d/%d", num, N_PRIOS );
#endif

        // this should never happen - if nothing else, the
        // idle process should be on the "Deferred" queue
        assert( num < N_PRIOS );

        // OK, we found a queue; pull the first process from it
        Payload p;
        assert( _q_remove( _readyQueue[num], &p ) );

        _readyLength[num] -= 1; // can't forget this!

        // get the PCB pointer
        pcb = (Pcb *) (p.data);
#if TRACING_DISPATCH
    _pcb_dump( "** got:", pcb );
#endif

        // if this process has been terminated, we need to clean
        // it up, then loop and pick another process; otherwise,
        // leave the loop
        if( pcb->state == Killed ) {
#if TRACING_DISPATCH
    __cio_puts( " KILLED\n" );
#endif
            // clean it up
            _performExit( pcb );
        } else {
            // found a good choice
            break;
        }

    } while( 1 );

    // we have a winner; set its state and remaining quantum
    pcb->state = Running;
    pcb->quantum = QUANTUM_DEFAULT;

    // make this the current process
    _current = pcb;
#if TRACING_DISPATCH
    _pcb_dump( "** Dispatched", _current );
#endif
}
