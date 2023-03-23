/**
** @file processes.c
**
** @author CSCI-452 class of 20225
**
** Process module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "kernel.h"
#include "processes.h"
#include "sched.h"
#include "vector.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// PCB management
static Vector _freePCBs;

/*
** PUBLIC GLOBAL VARIABLES
*/

// next available PID
pid_t _nextPid;

// active process count
uint32_t _nProcs;

// table of active processes
Pcb *_activeProcs[ MAX_PROCS ];

// pointer to the PCB for init()
Pcb *_initPCB;

// pointer to the "user shell" (if there is one)
Pcb *_userShell;

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
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
void _pcb_init( void ) {

    __cio_puts( " Processes" );

    // allocate an initial slice of PCBs
    _vec_clear( &_freePCBs );
    assert( _grow_list(SZ_PCB,_pcb_free) );   // returns 0 on failure

    // reset the "active" variables
    _nProcs = 0;
    CLEAR( _activeProcs );

    // first process is init, PID 1; it's created by system initialization

    // second process will be PID 2 - typically, idle()
    _nextPid = 2;
}

/*
** PCB manipulation
*/

/**
** _pcb_alloc() - allocate a PCB
**
** Allocates a PCB structure and returns it to the caller.
**
** @return a pointer to the allocated PCB, or NULL
*/
Pcb *_pcb_alloc( void ) {

#if TRACING_PCB
    __cio_puts( "** _pcb_alloc()\n" );
    _vec_dump( "** Free vec:", &_freePCBs, false );
#endif

    // see if there is an available PCB
    if( _freePCBs.occupancy == NULL ) {
        // try to repopulate the free list
        if( !_grow_list(SZ_PCB,_pcb_free) ) {
#if TRACING_PCB
    __cio_puts( "** ALLOC FAILED\n" );
#endif
            return NULL;
        }
    }

    // OK, we know there is at least one free PCB
    Pcb *new = (Pcb *) _vec_remove( &_freePCBs );

    // clear out the fields in this one just to be safe
    CLEAR_PTR( new );

#if TRACING_PCB
    __cio_printf( " -> %08x\n", (uint32_t) new );
    _vec_dump( "** Free vec:", &_freePCBs, false );
#endif

    // pass it back to the caller
    return( new );
}

/**
** _pcb_free() - return a PCB to the free list
**
** Deallocates the supplied PCB
**
** @param data   The PCB to be put on the free list
*/
void _pcb_free( void *data ) {

#if TRACING_PCB
    __cio_printf( "** _pcb_free(%08x)\n", (uint32_t) data );
#endif

    // sanity check!
    if( data == NULL ) {
        return;
    }

    // mark it as unused and unknown (just in case)
    Pcb *pcb = (Pcb *) data;
    pcb->state = Unused;
    pcb->pid = pcb->ppid = 0;

    // add it to the free list
    _vec_add( &_freePCBs, 0, pcb );

#if TRACING_PCB
    __cio_printf( "** _pcb_free(%08x)\n", (uint32_t) pcb );
    _vec_dump( "** Free vec:", &_freePCBs, false );
#endif
}

/**
** _pcb_find() - locate a PCB by PID
**
** @param pid   The PID we want to locate
**
** @return pointer to the desired PCB, or NULL
*/
Pcb *_pcb_find( pid_t pid ) {

#if TRACING_PCB
    __cio_printf( "** _pcb_find(%u)", pid );
#endif

    // iterate through the PCB table
    for( int i = 0; i < MAX_PROCS; ++i ) {
        // if this is the one we want, we're done
        if( _activeProcs[i] != NULL && _activeProcs[i]->pid == pid ) {
#if TRACING_PCB
    __cio_printf( ", slot #%d\n", i );
#endif
            return( _activeProcs[i] );
        }
    }

#if TRACING_PCB
    __cio_puts( ", not found\n" );
#endif
    return( NULL );
}

/**
** _pcb_cleanup(pcb) - reclaim a process' data structures
**
** @param pcb   The PCB to reclaim
*/
void _pcb_cleanup( Pcb *pcb ) {

#if TRACING_PCB
    __cio_printf( "** _pcb_cleanup() cleaning 0x%08x\n", (uint32_t) pcb );
#endif

    // avoid deallocating a NULL pointer
    if( pcb == NULL ) {
        // should this be an error?
        return;
    }

    // clear the entry in the process table
    int i;
    for( i = 0; i < MAX_PROCS; ++i ) {
        if( _activeProcs[i] == pcb ) {
            _activeProcs[i] = NULL;
            --_nProcs;
            break;
        }
    }

    // just to be safe....
    assert2( i < MAX_PROCS );

    // release the stack(en?)
    if( pcb->stack != NULL ) {
        _stk_free( pcb->stack );
    }

    // release the PCB
    pcb->state = Unused;        // just to be sure!
    pcb->pid = pcb->ppid = 0;
    _pcb_free( pcb );
#if TRACING_PCB
    _vec_dump( "** PCB freevec:", &_freePCBs, false );
#endif
}

/**
** _performExit - do the real work for exit() and some kill() calls
**
** Also called from the scheduler and the dispatcher.
**
** @param victim  Pointer to the PCB for the exiting process
*/
void _performExit( Pcb *victim ) {

#if TRACING_EXIT
    __cio_printf( "** perfExit, victim PCB %08x pid %u ppid %u\n",
            (uint32_t) victim, victim->pid, victim->ppid );
#endif

    // set its state
    victim->state = Zombie;

    /*
    ** We need to locate the parent of this process.  We also need
    ** to reparent any children of this process.  We do these in
    ** a single loop.
    */
    Pcb *parent = NULL;
    Pcb *zombie = NULL;

    for( int i = 0; i < MAX_PROCS; ++i ) {
        register Pcb *curr = _activeProcs[i];

        // make sure this is a valid entry
        if( curr == NULL ) {
            continue;
        }

        // see if this is our parent; if it isn't, see if
        // it's a child of the terminating process
        if( curr->pid == victim->ppid ) {

            // found the parent!
            parent = curr;
#if TRACING_EXIT
    __cio_printf( "** perfExit parent PCB %08x pid %u\n",
            (uint32_t) parent, parent->pid );
#endif
            // NOTE: we don't break out of the 'for' loop
            // here because we want to continue iterating
            // to find all the children of this processes
        } else if( curr->ppid == victim->pid ) {
#if TRACING_EXIT
    __cio_printf( "** perfExit child PCB %08x pid %u",
            (uint32_t) curr, curr->pid );
#endif

            // found a child - reparent it
            curr->ppid = PID_INIT;

            // see if this child is already undead
            if( curr->state == Zombie ) {
                // if it's already a zombie, remember it, so we
                // can pass it on to 'init'; also, if there are
                // two or more zombie children, it doesn't matter
                // which one we pick here, as the others will be
                // collected as 'init' loops
                zombie = _activeProcs[i];
#if TRACING_EXIT
    __cio_puts( " is a zombie\n" );
#endif
            }
#if TRACING_EXIT
    __cio_puts( " reparented\n" );
#endif
        }
    }

    // every process must have a parent, even if it's 'init'
    assert( parent != NULL );

    /*
    ** If we found a child that was already terminated, we need to
    ** wake up the init process if it's already waiting.
    **
    ** Note: we only need to do this for one Zombie child process -
    ** init will loop and collect the others after it finishes with
    ** this one.
    */

    if( zombie != NULL && _initPCB->state == Waiting ) {

        // *****************************************************
        // This code assumes that Waiting processes are *not* in
        // a queue, but instead are just in the process table with
        // a state of 'Waiting'.  This simplifies life immensely,
        // because we don't need to dequeue it - we can just
        // schedule it and let it go.
        // *****************************************************

        // intrinsic return value is the PID
        RET(_initPCB) = zombie->pid;

        // may also want to return the exit status
        int32_t *ptr = (int32_t *) ARG(_initPCB,1);
        if( ptr != NULL ) {
            // *****************************************************
            // Potential VM issue here!  This code assigns the exit
            // status into a variable in the parent's address space.
            // This works in the baseline because we aren't using
            // any type of memory protection.  If address space
            // separation is implemented, this code will very likely
            // STOP WORKING, and will need to be fixed.
            // *****************************************************
            *ptr = zombie->status;
        }
#if TRACING_EXIT
    __cio_printf( "** perfExit first zombie %d given to init\n",
            zombie->pid );
#endif

        // all done - schedule 'init', and clean up the zombie
        _schedule( _initPCB );
        _pcb_cleanup( zombie );
    }

    // if the parent is already waiting, wake it up
    if( parent->state == Waiting ) {

        // intrinsic return value is the PID
        RET(parent) = victim->pid;

        // may also want to return the exit status
        int32_t *ptr = (int32_t *) ARG(parent,1);
        if( ptr != NULL ) {
            // *****************************************************
            // Potential VM issue here!  This code assigns the exit
            // status into a variable in the parent's address space.
            // This works in the baseline because we aren't using
            // any type of memory protection.  If address space
            // separation is implemented, this code will very likely
            // STOP WORKING, and will need to be fixed.
            // *****************************************************
            *ptr = victim->status;
        }
#if TRACING_EXIT
   __cio_printf( "** perfExit victim %d given to parent %d\n",
            victim->pid, parent->pid );
#endif

        // all done - schedule the parent, and clean up the zombie
        _schedule( parent );
        _pcb_cleanup( victim );

    } else {

        // parent isn't waiting, so we become a Zombie

        // *****************************************************
        // This code assumes that Zombie processes are *not* in
        // a queue, but instead are just in the process table with
        // a state of 'Zombie'.  This simplifies life immensely,
        // because we won't need to dequeue it when it is collected
        // by its parent.
        // *****************************************************

        victim->state = Zombie;

    }

    /*
    ** Note: we don't call _dispatch() here - we leave that for
    ** the calling routine, as it's possible we don't need to
    ** choose a new current process.
    */
}

/**
** _pcount - count the number of active processes in each state
**
** @param counts  Array into which the counts are placed
**
** @return The number of empty process table slots
*/
int _pcount( register uint32_t *counts ) {

    // clear all the counts
    // (can't use CLEAR_PTR here because it's an array of things)
    __memclr( (void *) counts, N_STATES * sizeof(uint32_t) );

    // generate the frequency counts
    int unused = 0;
    for( register int i = 0; i < MAX_PROCS; ++i ) {
        if( _activeProcs[i] != NULL ) {
            // active process
            counts[_activeProcs[i]->state] += 1;
        } else {
            // empty slot
            ++unused;
        }
    }

    return( unused );
}

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
void _pcb_dump( const char *msg, register Pcb *p ) {

    // first, the message (if there is one)
    if( msg ) {
        __cio_puts( msg );
    }

    // the pointer
    __cio_printf( " @ %08x: ", (uint32_t) p );

    // if it's NULL, why did you bother calling me?
    if( p == NULL ) {
        __cio_puts( " NULL???\n" );
        return;
    }

    // now, the contents
    __cio_printf( " pids %d/%d state %d prio %d",
                  p->pid, p->ppid, p->state, p->priority );

    __cio_printf( "\n quant %d xit %d wake %08x",
                  p->quantum, p->status, p->wakeup );

    __cio_printf( "\n context %08x stack %08x",
                  (uint32_t) p->context, (uint32_t) p->stack );

    // and the filler (just to be sure)
    __cio_puts( " fill: " );
    for( int i = 0; i < sizeof(p->filler); ++i ) {
        __cio_printf( "%02x", p->filler[i] );
    }
    __cio_putchar( '\n' );
}

/**
** _ctx_dump(msg,context)
**
** Dumps the contents of this process context to the console
**
** @param msg   An optional message to print before the dump
** @param c     The context to dump out
*/
void _ctx_dump( const char *msg, register Context *c ) {

    // first, the message (if there is one)
    if( msg ) {
        __cio_puts( msg );
    }

    // the pointer
    __cio_printf( " @ %08x: ", (uint32_t) c );

    // if it's NULL, why did you bother calling me?
    if( c == NULL ) {
        __cio_puts( " NULL???\n" );
        return;
    }

    // now, the contents
    __cio_printf( "ss %04x gs %04x fs %04x es %04x ds %04x cs %04x\n",
                  c->ss & 0xff, c->gs & 0xff, c->fs & 0xff,
                  c->es & 0xff, c->ds & 0xff, c->cs & 0xff );
    __cio_printf( "  edi %08x esi %08x ebp %08x esp %08x\n",
                  c->edi, c->esi, c->ebp, c->esp );
    __cio_printf( "  ebx %08x edx %08x ecx %08x eax %08x\n",
                  c->ebx, c->edx, c->ecx, c->eax );
    __cio_printf( "  vec %08x cod %08x eip %08x eflags %08x\n",
                  c->vector, c->code, c->eip, c->eflags );
}

/**
** _ctx_dump_all(msg)
**
** dump the process context for all active processes
**
** @param msg  Optional message to print
*/
void _ctx_dump_all( const char *msg ) {

    if( msg != NULL ) {
        __cio_puts( msg );
    }

    __cio_printf( " %d active processes\n", _nProcs );

    if( _nProcs < 1 ) {
        return;
    }

    int n = 0;
    for( int i = 0; i < MAX_PROCS; ++i ) {
        Pcb *pcb = _activeProcs[i];
        if( pcb != NULL && pcb->state != Unused ) {
            ++n;
            __cio_printf( "%2d[%2d]: ", n, i );
            _ctx_dump( NULL, pcb->context );
        }
    }
}

/**
** _ptable_dump(msg,all)
**
** dump the contents of the "active processes" table
**
** @param msg  Optional message to print
** @param all  Dump all or only part of the relevant data
*/
void _ptable_dump( const char *msg, bool_t all ) {

    if( msg ) {
        __cio_puts( msg );
    }
    __cio_putchar( ' ' );

    int used = 0;
    int empty = 0;

    for( int i = 0; i < MAX_PROCS; ++i ) {
        register Pcb *pcb = _activeProcs[i];
        if( pcb == NULL ) {

            // an empty slot
            ++empty;

        } else {

            // a non-empty slot
            ++used;

            // if not dumping everything, add commas if needed
            if( !all && used ) {
                __cio_putchar( ',' );
            }

            // things that are always printed
            __cio_printf( " #%d: %d/%d", i, pcb->pid, pcb->ppid );
            if( pcb->state < Unused || pcb->state >= N_STATES ) {
                __cio_printf( " UNKNOWN" );
            } else {
                __cio_printf( " %s", _stateStr[pcb->state][S_NAME] );
            }
            // do we want more info?
            if( all ) {
                __cio_printf( " wk %08x stk %08x ESP %08x EIP %08x\n",
                        pcb->wakeup, (uint32_t) pcb->stack,
                        pcb->context->esp,
                        pcb->context->eip );
            }
        }
    }
    // only need this if we're doing one-line output
    if( !all ) {
        __cio_putchar( '\n' );
    }

    // sanity check - make sure we saw the correct number of table slots
    if( (used + empty) != MAX_PROCS ) {
        __cio_printf( "Table size %d, used %d + empty %d = %d???\n",
                      MAX_PROCS, used, empty, used + empty );
    }
}
