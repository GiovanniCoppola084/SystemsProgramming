/**
** @file syscalls.c
**
** @author CSCI-452 class of 20225
**
** System call implementations
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "x86arch.h"
#include "x86pic.h"
#include "uart.h"

#include "support.h"
#include "bootstrap.h"

#include "syscalls.h"
#include "sched.h"
#include "processes.h"
#include "stacks.h"
#include "clock.h"
#include "cio.h"
#include "sio.h"

/*
** PRIVATE DEFINITIONS
*/

// macros to simplify tracing a bit
//
// TRACING_SYSCALLS and TRACING_SYSRETS are defined in kdefs.h
// If not tracing syscalls, SYSCALL_ENTER is a no-op, and
// SYSCALL_EXIT just does a return.

#if TRACING_SYSCALLS
#define SYSCALL_ENTER(x)   do { \
        __cio_printf( "--> %s, pid %08x", __func__, (uint32_t) (x) ); \
    } while(0)
#else
#define SYSCALL_ENTER(x)   /* */
#endif

#if TRACING_SYSRETS
#define SYSCALL_EXIT(x)    do { \
        __cio_printf( "<-- %s %08x\n", __func__, (uint32_t) (x) ); \
        return; \
    } while(0)
#else
#define SYSCALL_EXIT(x)    return
#endif

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// The system call jump table
//
// Initialized by _sys_init() to ensure that the code::function mappings
// are correct even if the code values should happen to change.

static void (*_syscalls[N_SYSCALLS])( Pcb *);

/*
** PUBLIC GLOBAL VARIABLES
*/

// queue of sleeping processes
Queue _sleepQueue;

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  _sys_isr
**
** System call ISR
**
** @param vector    Vector number for the clock interrupt
** @param code      Error code (0 for this interrupt)
*/
static void _sys_isr( int vector, int code ) {

    // Keep the compiler happy.
    (void) vector;
    (void) code;

    // If there is no current process, we're in deep trouble.
    assert( _current != NULL );

    // Much less likely to occur, but still potentially problematic.
    assert1( _current->context != NULL );

    // Retrieve the system call code.
    uint32_t syscode = REG( _current, eax );

#if TRACING_SYSCALLS
    __cio_printf( "** SYS pid %u code %u\n", _current->pid, syscode );
#endif

    // Validate the code.
    if( syscode >= N_SYSCALLS ) {
        // Uh-oh....
        __sprint( b256, "pid %d bad syscall 0x%x", _current->pid, syscode );
        WARNING( b256 );
        // Force a call to exit().
        syscode = SYS_exit;
        ARG(_current,1) = E_BAD_SYSCALL;
#if TRACING_SYSCALLS
    __cio_printf( "** SYS pid %u bad code\n", _current->pid );
#endif
    }

    // Handle the system call.
    _syscalls[syscode]( _current );

    // Tell the PIC we're done.
    __outb( PIC_PRI_CMD_PORT, PIC_EOI );
}

/**
** Second-level syscall handlers
**
** All have this prototype:
**
**    static void _sys_NAME( Pcb * );
**
** Values being returned to the user are placed into the EAX
** field in the context save area for that process.
*/

/**
** _sys_exit - terminate the calling process
**
** implements:
**      void exit( int32_t status );
**
** does not return
*/
static void _sys_exit( Pcb *pcb ) {

    SYSCALL_ENTER( pcb->pid );

    // set the termination status for this process
    pcb->status = ARG(pcb,1);

    // perform all necessary exit processing
    _performExit( pcb );

    // need a new current process
    _dispatch();

    SYSCALL_EXIT( _current );

}

/**
** _sys_spawn - create a new process
**
** implements:
**      pid_t spawn( void *entry, prio_t prio, uint32_t len, void *arg );
**
** returns:
**      parent - PID of new child, or -1 on error
**      child  - 0
*/
static void _sys_spawn( Pcb *pcb ) {

    SYSCALL_ENTER( pcb->pid );

    // Make sure there's room for another process!
    if( _nProcs >= MAX_PROCS ) {
        // No room at the inn!
        RET(pcb) = E_NO_PROCS;
        SYSCALL_EXIT( E_NO_PROCS );
    }

    // verify that the priority is legal
    prio_t prio = ARG(pcb,2);
    if( prio < pcb->priority ) {
        RET(pcb) = E_BAD_PRIO;
        SYSCALL_EXIT( E_BAD_PRIO );
    }

    // First, allocate a PCB.
    Pcb *new = _pcb_alloc();

    // if we're really checking things, this will
    // blow up; otherwise, we'll just return
    // "no processes" to the caller
    assert1( new != NULL );

    if( new == NULL ) {
        RET(pcb) = E_NO_PROCS;
        SYSCALL_EXIT( E_NO_PROCS );
    }

    // Create the stack for the child.
    new->stack = _stk_alloc();

    assert1( new->stack != NULL );

    if( new->stack == NULL ) {
        _pcb_free( new );
        RET(pcb) = E_NO_PROCS;
        SYSCALL_EXIT( E_NO_PROCS );
    }

    // Set the child's identity.
    new->pid = _nextPid++;
    new->ppid = pcb->pid;
    new->state = New;
    new->priority = prio;

    /*
    ** Now, we need to set up the stack for the new process.
    ** We call a support function for this, as we need to do
    ** this in several places.
    **
    ** The stack setup function will take care of putting the
    ** argument string into the stack and setting up the initial
    ** context save area.  We will then fill in the details.
    */

    // collect the other parameters
    uint32_t entry = ARG(pcb,1);
    uint32_t len   = ARG(pcb,3);
    void *buffer   = (void *) ARG(pcb,4);

    Context *ctx = _stk_setup( new->stack, len, buffer );
    assert1( ctx != NULL );

    // remember where the context area is
    new->context = ctx;

    // clear it out
    CLEAR_PTR(ctx);

    // fill in the critical register values
    ctx->eflags = DEFAULT_EFLAGS;
    ctx->esp = (uint32_t) (ctx + 1);    // first byte after the context
    ctx->eip = entry;
    ctx->cs = GDT_CODE;
    ctx->ss = GDT_STACK;
    ctx->ds = ctx->es = ctx->fs = ctx->gs = GDT_DATA;

    // Set the return values for the two processes.
    RET(pcb) = new->pid;
    RET(new) = 0;

    // Add the new process to the process table.
    int ix;
    for( ix = 0; ix < MAX_PROCS; ++ix ) {
        if( _activeProcs[ix] == NULL ) {
            break;
        }
    }

    // Did we find an open slot?
    if( ix >= MAX_PROCS ) {
        PANIC( 0, "no empty slot in non-full process table" );
    }

    // Yes - record the new process.
    _activeProcs[ix] = new;
    ++_nProcs;

    // Schedule the child, and let the parent continue.
    _schedule( new );

    SYSCALL_EXIT( new->pid );
}

/**
** _sys_read - read into a buffer from a stream
**
** implements:
**      int32_t read( int chan, void *buffer, uint32_t length );
**
** returns:
**      input data (in 'buffer')
**      number of bytes read, or an error code (intrinsic)
*/
static void _sys_read( Pcb *pcb ) {
    int n = 0;
    char *buf = (char *) ARG(pcb,2);
    uint32_t length = ARG(pcb,3);

    SYSCALL_ENTER( pcb->pid );

    // try to get the next character(s)
    switch( ARG(pcb,1) ) {
    case CHAN_CIO:
        // console input is non-blocking
        if( __cio_input_queue() < 1 ) {
            RET(pcb) = E_NO_DATA;
            SYSCALL_EXIT( E_NO_DATA );
        }
        // at least one character
        n = __cio_gets( buf, length );
        break;

    case CHAN_SIO:
        // this may block the process; if so, _sio_reads() will
        // dispatch a new one, and we won't return to this point
        n = _sio_reads( buf, length );
        break;

    default:
        // bad channel code
        RET(pcb) = E_BAD_CHAN;
        SYSCALL_EXIT( E_BAD_CHAN );
    }

    // if there was data, return the byte count to the process;
    // otherwise, block the process until data is available
    if( n > 0 ) {

        RET(pcb) = n;
        SYSCALL_EXIT( n );

    } else {

        // mark it as blocked
        pcb->state = Blocked;

        // put it on the SIO input queue
        assert1( _q_insert(_sioReadQueue,(void *)pcb,0) );

        // select a new current process
        _dispatch();
    }
}

/**
** _sys_write - write from a buffer to a stream
**
** implements:
**      int32_t write( int chan, const void *buffer, uint32_t length );
**
** returns:
**      number of bytes written, or an error code (intrinsic)
*/
static void _sys_write( Pcb *pcb ) {
    uint32_t chan = ARG(pcb,1);
    char *buf = (char *) ARG(pcb,2);
    uint32_t length = ARG(pcb,3);

    SYSCALL_ENTER( pcb->pid );

    // this is almost insanely simple, but it does separate the
    // low-level device access fromm the higher-level syscall implementation

    switch( chan ) {
    case CHAN_CIO:
        __cio_write( buf, length );
        RET(pcb) = length;
        break;

    case CHAN_SIO:
        _sio_write( buf, length );
        RET(pcb) = length;
        break;

    default:
        RET(pcb) = E_BAD_CHAN;
        break;
    }
    SYSCALL_EXIT( RET(pcb) );
}

/**
** _sys_sleep - put the current process to sleep for some length of time
**
** implements:
**      void sleep( uint32_t ms );
**
** if ms == 0, just yields the CPU
*/
static void _sys_sleep( Pcb *pcb ) {
    uint32_t ms = ARG(pcb,1);

    SYSCALL_ENTER( pcb->pid );

    // does the process just want to yield?
    if( ms == 0 ) {

        // yes, so just schedule it
        _schedule( pcb );

    } else {

        // no, so we must put it to sleep
        pcb->state = Sleeping;

        // we treat the user shell process differently; 
        // it doesn't actually get put onto the sleep
        // queue - instead, it just hangs out waiting
        // for console keyboard input to schedule it
        // again
        if( pcb != _userShell ) {
            pcb->wakeup = _systemTime + MS_TO_TICKS(ms);
            ms = _q_insert( _sleepQueue, (void *) pcb, pcb->wakeup );
        }

    }

    // regardless, we need to pick a new current process
    _dispatch();

    SYSCALL_EXIT( (uint32_t) _current );
}

/**
** _sys_kill - terminate a process with extreme prejudice
**
** implements:
**      status_t kill( pid_t victim );
**
** returns:
**      status of the kill attempt
*/
static void _sys_kill( Pcb *pcb ) {

    SYSCALL_ENTER( pcb->pid );

    // POTENTIAL DANGER:  What if we try kill(init) or kill(idle)?
    // Might want to guard for that here!

    // kill(0) is a request to kill the calling process
    pid_t vicID = ARG(pcb,1);
    if( vicID == 0 ) {
        vicID = pcb->pid;
    }

    // locate the victim
    Pcb *victim = NULL;
    for( int i = 0; i < MAX_PROCS; ++i ) {
        if( _activeProcs[i] != NULL && _activeProcs[i]->pid == vicID ) {
            victim = _activeProcs[i];
            break;
        }
    }

    // did we find the victim?
    if( victim == NULL ) {
        // nope!
        RET(pcb) = E_NOT_FOUND;
        SYSCALL_EXIT( E_NOT_FOUND );
    }

    // found the victim!  take care of it now

    // how we process the victim depends on its current state:
    switch( victim->state ) {

        /*
        ** for the first three of these states, the process is on
        ** a queue somewhere; just mark it as 'Killed', and when it
        ** comes off that queue via _schedule() or _dispatch() we
        ** will clean it up
        */

    case Ready:     // FALL THROUGH
    case Sleeping:  // FALL THROUGH
    case Blocked:
        // mark it
        victim->status = E_KILLED;
        // deal with its children and its parent
        _performExit( victim );
        RET(pcb) = E_SUCCESS;
        break;

    case Running:  // current process
        // we have met the enemy, and it is us!
        pcb->status = E_KILLED;
        _performExit( pcb );
        // need a new 'current process"
        _dispatch();
        break;

    case Waiting:
        // much like 'Running', except that it's not the current
        // process, so we don't have to dispatch another one
        victim->status = E_KILLED;
        _performExit( victim );
        RET(pcb) = E_SUCCESS;
        break;

    case Killed:    // FALL THROUGH
    case Zombie:
        // you can't kill something if it's already dead
        RET(pcb) = E_SUCCESS;
        break;

    default:
        // this is a really bad potential problem - we have an unexpected
        // (or bogus) process state, so we report that
        __sprint( b256, "*** kill(): victim %d, unknown state %d\n",
                victim->pid, victim->state );
        PANIC( 0, b256 );
    }

    
    SYSCALL_EXIT( RET(pcb) );
}

/**
** _sys_waitpid - wait for a child process to terminate
**
** implements:
**      pid_t waitpid( pid_t id, int32_t *status );
**
** returns:
**      pid of the terminated child, or E_NO_CHILDREN (intrinsic)
**      exit status of the child via a non-NULL 'status' parameter
*/
static void _sys_waitpid( Pcb *pcb ) {

    SYSCALL_ENTER( pcb->pid );

    /*
    ** We need to do two things here:  (1) find out whether or
    ** not this process has any children in the system, and (2)
    ** find out whether the desired child (or any child, if the
    ** target PID is 0) has terminated.
    **
    ** To do this, we loop until we find a the requested PID or
    ** a Zombie child process, or have gone through all of the
    ** slots in the process table.
    **
    ** If the target PID is 0, we don't care which child process
    ** we reap here; there could be several, but we only need to
    ** find one.
    */

    // verify that we aren't looking for ourselves!
    pid_t target = ARG(pcb,1);

    if( target == pcb->pid ) {
        RET(pcb) = E_BAD_PARAM;
        SYSCALL_EXIT( E_BAD_PARAM );
    }

    // Good.  Now, figure out what we're looking for.

    Pcb *child = NULL;

    if( target != 0 ) {

        // we're looking for a specific child
        child = _pcb_find( target );

        if( child != NULL ) {
            
            // found the process; is it one of our children:
            if( child->ppid != pcb->pid ) {
                // NO, so we can't wait for it
                RET(pcb) = E_BAD_PID;
                SYSCALL_EXIT( E_BAD_PID );
            }

            // yes!  is this one ready to be collected?
            if( child->state != Zombie ) {
                // no, so we'll have to block for now
                child = NULL;
            }

        } else {

            // no such child
            RET(pcb) = E_BAD_PID;
            SYSCALL_EXIT( E_BAD_PID );

        }

    } else {

        // looking for any child
        int i;

        // we need to find a process that is our child
        // and has already exited

        child = NULL;
        bool_t found = false;

        for( i = 0; i < MAX_PROCS; ++i ) {
            // save a bit of time
            register Pcb *curr = _activeProcs[i];

            if( curr != NULL && curr->ppid == pcb->pid ) {

                // found one!
                found = true;

                // has it already exited?
                if( child->state == Zombie ) {
                    // yes, so we're done here
                    child = curr;
                    break;
                }
            }
        }

        if( !found ) {
            // got through the loop without finding a child!
            RET(pcb) = E_NO_CHILDREN;
            SYSCALL_EXIT( E_NO_CHILDREN );
        }

    }

    /*
    ** At this point, one of these situations is true:
    **
    **    * we are looking for a specific child and found it
    **    * we are looking for any child and found one
    **
    ** Either way, 'child' will be non-NULL if the selected
    ** process has already become a Zombie.  If that's the
    ** case, we collect its status and clean it up; otherwise,
    ** we block this process.
    */

    // did we find one to collect?
    if( child == NULL ) {

        // no - mark the parent as "Waiting"
        pcb->state = Waiting;

        // select a new current process
        _dispatch();
        SYSCALL_EXIT( _current );
    }

    // found a Zombie; collect its information and clean it up
    RET(pcb) = child->pid;
    int32_t *stat = (int32_t *) ARG(pcb,2);

    // if stat is NULL, the parent doesn't want the status
    if( stat != NULL ) {
        *stat = child->status;
    }

    // clean up the child
    _pcb_cleanup( child );

    SYSCALL_EXIT( RET(pcb) );
}

/**
** _sys_ushell - register this process as the "user shell" process
**
** implements:
**      int ushell( void );
*/
static void _sys_ushell( Pcb *pcb ) {
    uint32_t result = 0;

    SYSCALL_ENTER( pcb->pid );

    // is a user shell already registered?
    if( _userShell == NULL ) {

        // no - register this one
        _userShell = pcb;
        __cio_printf( "!!! PID %u registered as user shell\n", pcb->pid );

    } else {

        // yes - tell the caller who it is
        result = _userShell->pid;

    }

    RET(pcb) = result;
    SYSCALL_EXIT( result );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _ord_wakeup
**
** Ordering function for the sleep queue
**
** @param k1    First key value to examine
** @param k2    Second key value to examine
**
** @return Relationship between the key values:
**      < 0   k1 < k2
**      = 0   k1 == k2
**      > 0   k1 > k2
*/
static int _ord_wakeup( key_t k1, key_t k2 ) {

    // QUESTION:  does it make more sense to put this function
    // somewhere else with a name like "_ord_uint32", which would
    // make it more generic and thus usable for other ordered
    // queues which sort in ascending order by uint32_t values?

    if( k1 < k2 )
        return( -1 );
    else if( k1 == k2 )
        return( 0 );
    else
        return( 1 );
}

/**
** Name:  _sys_init
**
** Syscall module initialization routine
**
** Dependencies:
**    Must be called after _cio_init()
*/
void _sys_init( void ) {

    __cio_puts( " Syscall" );

    /*
    ** Set up the syscall jump table.  We do this here
    ** to ensure that the association between syscall
    ** code and function address is correct even if the
    ** codes change.
    */

    _syscalls[ SYS_exit ]     = _sys_exit;
    _syscalls[ SYS_spawn ]    = _sys_spawn;
    _syscalls[ SYS_read ]     = _sys_read;
    _syscalls[ SYS_write ]    = _sys_write;
    _syscalls[ SYS_sleep ]    = _sys_sleep;
    _syscalls[ SYS_kill ]     = _sys_kill;
    _syscalls[ SYS_waitpid ]  = _sys_waitpid;
    _syscalls[ SYS_ushell ]   = _sys_ushell;

    // allocate a queue for sleeping processes
    _sleepQueue = _q_alloc( _ord_wakeup );

    // install the second-stage ISR
    __install_isr( INT_VEC_SYSCALL, _sys_isr );
}
