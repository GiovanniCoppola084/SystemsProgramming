/**
** @file kernel.c
**
** @author Numerous CSCI-452 classes
**
** Miscellaneous OS support routines.
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "kernel.h"
#include "queues.h"
#include "processes.h"

#include "ahci.h"
#include "bootstrap.h"
#include "cio.h"
#include "clock.h"
#include "kmem.h"
#include "pio.h"
#include "sched.h"
#include "sio.h"
#include "support.h"
#include "syscalls.h"

// need address of the init() function
extern int32_t init( uint32_t, void * );

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

// character buffers, usable throughout the OS
// not guaranteed to retain their contents across an exception return
char b256[256];
char b512[512];

// Other kernel variables that could be defined here:
//
//     system time
//     pointer to the current process
//     information about the initial process
//         pid, PCB pointer
//     information about the idle process (if there is one)
//         pid, PCB pointer
//     information about active processes
//         static array of PCBs, active count, next available PID
//     queue variables
//     OS stack & stack pointer
//

// A separate stack for the OS itself
// (NOTE:  this assumes the OS is not reentrant!)
Stack *_systemStack;
uint32_t *_systemESP;

// table of state name strings
// [0]: long name, [1]: short name
const char *_stateStr[N_STATES][2] = {
    [ Unused   ] = { "Unused",   "Unu" },
    [ New      ] = { "New",      "New" },
    [ Ready    ] = { "Ready",    "Rdy" },
    [ Running  ] = { "Running",  "Run" },
    [ Sleeping ] = { "Sleeping", "Slp" },
    [ Waiting  ] = { "Waiting",  "Wtg" },
    [ Blocked  ] = { "Blocked",  "Blk" },
    [ Zombie   ] = { "Zombie",   "Zom" },
    [ Killed   ] = { "Killed"    "Kil" }
};

// table of priority name strings
const char *_prioStr[N_PRIOS] = {
    [ System   ] = "System",
    [ User     ] = "User",
    [ Deferred ] = "Deferred"
};

/*
** PRIVATE FUNCTIONS
*/

/**
** _kreport - report the system configuration
**
** Prints configuration information about the OS on the console monitor.
**
** @param dtrace  Decode the TRACE options
*/
void _kreport( bool_t dtrace ) {

    __cio_puts( "\n-------------------------------\n" );
    __cio_printf( "Config:  MAX_PROCS = %d", MAX_PROCS );
    __cio_printf( " N_PRIOS = %d", N_PRIOS );
    __cio_printf( " N_STATES = %d", N_STATES );
    __cio_printf( " CLOCK = %dHz\n", CLOCK_FREQUENCY );

    // This code is ugly, but it's the simplest way to
    // print out the values of compile-time options
    // without spending a lot of execution time at it.

    __cio_puts( "Options: " );
#ifdef RPT_INT_UNEXP
    __cio_puts( " U_Ints" );
#endif
#ifdef RPT_INT_MYSTERY
    __cio_puts( " M_Ints" );
#endif
#ifdef TRACE_CX
    __cio_puts( " CX" );
#endif
#ifdef SANITY
    __cio_printf( " SANITY = %d", SANITY );
#endif
#ifdef USER_SHELL
    __cio_puts( " Ushell" );
#endif
#ifdef CONSOLE_STATS
    __cio_puts( " Cstats" );
#endif
#ifdef STATUS
    __cio_printf( " STATUS = %d", STATUS );
#endif

#ifdef TRACE
    __cio_printf( " TRACE = 0x%03x\n", TRACE );

    // decode the trace settings if that was requested
    if( TRACING_SOMETHING && dtrace ) {

        // this one is simpler - we rely on string literal
        // concatenation in the C compiler to create one
        // long string to print out

        __cio_puts( "Tracing:"
#   if TRACING_PCB
            " PCB"
#   endif
#   if TRACING_STACK
             " STK"
#   endif
#   if TRACING_QUEUE
             " QUE"
#   endif
#   if TRACING_SCHED
             " SCHED"
#   endif
#   if TRACING_SYSCALLS
             " SYSCALL"
#   endif
#   if TRACING_SYSRETS
             " SYSRET"
#   endif
#   if TRACING_EXIT
             " EXIT"
#   endif
#   if TRACING_DISPATCH
             " DISPATCH"
#   endif
#   if TRACING_CONSOLE
             " CONS"
#   endif
#   if TRACING_KMEM
             " KM"
#   endif
#   if TRACING_KMEM_FREE
             " KMFL"
#   endif
#   if TRACING_SPAWN
             " SPAWN"
#   endif
#   if TRACING_SIO_STAT
             " S_STAT"
#   endif
#   if TRACING_SIO_ISR
             " S_ISR"
#   endif
#   if TRACING_SIO_RD
             " S_RD"
#   endif
#   if TRACING_SIO_WR
             " S_WR"
#   endif
             ); // end of __cio_puts() call
    }
#endif

    __cio_puts( "\n-------------------------------\n" );
}


#if defined(CONSOLE_STATS) || defined(USER_SHELL)
/**
** _kshell - callback routine for console input
**
** Called by the CIO module when a key is pressed on the
** console keyboard.  Depending on the key, it will print
** statistics on the console display, or will cause the
** user shell process to be dispatched.
**
** This code runs as part of the CIO ISR.
*/
static void _kshell( int code ) {

    switch( code ) {

    case 'a':  // dump the active table
        _ptable_dump( "\nActive processes", false );
        break;

    case 'c':  // dump context info for all active PCBs
        _ctx_dump_all( "\nContext dump" );
        break;

    case 'p':  // dump the active table and all PCBs
        _ptable_dump( "\nActive processes", true );
        break;

    case 'q':  // dump the queues
        // code to dump out any/all queues
        _q_dump( "SleepQ", _sleepQueue );
#ifdef QNAME
        _q_dump( "SIOQ", QNAME );
#endif
        _q_dump( "RQ[S]", _readyQueue[System] );
        _q_dump( "RQ[U]", _readyQueue[User] );
        _q_dump( "RQ[D]", _readyQueue[Deferred] );
        break;

    case 'r':  // print system configuration information
        _kreport( true );
        break;

    case 's':  // dump stack info for all active PCBS
        __cio_puts( "\nActive stacks (w/5-sec. delays):\n" );
        for( register int i = 0; i < MAX_PROCS; ++i ) {
            register Pcb *pcb = _activeProcs[i];
            if( pcb != NULL && pcb->state != Unused ) {
                __cio_printf( "pid %5u: ", pcb->pid );
                __cio_printf( "EIP %08x, ", pcb->context->eip );
                _stk_dump( NULL, pcb->stack, 12 );
            }
        }
        break;

#ifdef USER_SHELL
        // schedule the "user shell" process if there is one
    case 'u':
        if( _userShell != NULL ) {
            _schedule( _userShell );
            // note:  we don't dispatch it immediately; we let
            // the current process finish its quantum, and then
            // the user shell will be dispatched at that time,
            // as it runs at 'System' priority
        } else {
            __cio_puts( "KSH: no user process to schedule" );
        }
        break;
#endif

        // ignore CR and LF
    case '\r': // FALL THROUGH
    case '\n':
        break;
 
    default:
        __cio_printf( "shell: unknown request '0x%02x'\n", code );
        // FALL THROUGH

    case 'h':  // help message
        __cio_puts( "\nCommands:\n"
                     "   a  -- dump the active table\n"
                     "   c  -- dump contexts for active processes\n"
                     "   h  -- this message\n"
                     "   p  -- dump the active table and all PCBs\n"
                     "   q  -- dump the queues\n"
                     "   r  -- print system configuration\n"
                     "   s  -- dump stacks for active processes\n"
#ifdef USER_SHELL
                     "   u  -- dispatch the user shell\n"
#endif
                    );
        break;
    }
}
#endif

/*
** PUBLIC FUNCTIONS
*/

/**
** _kinit - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/
void _kinit( void ) {

    /*
    ** BOILERPLATE CODE - taken from basic framework
    **
    ** Initialize interrupt stuff.
    */

    __init_interrupts();  // IDT and PIC initialization

    /*
    ** Console I/O system.
    **
    ** Does not depend on the other kernel modules, so we can
    ** initialize it before we initialize the kernel memory
    ** and queue modules.
    */

#if defined(CONSOLE_STATS) || defined(USER_SHELL)
    __cio_init( _kshell );
#else
    __cio_init( NULL );    // no console callback routine
#endif

#ifdef TRACE_CX
    // define a scrolling region in the top 7 lines of the screen
    __cio_setscroll( 0, 7, 99, 99 );
    // clear the top line
    __cio_puts_at( 0, 0, "*                                                                               " );
    // separator
    __cio_puts_at( 0, 6, "================================================================================" );
#endif

    /*
    ** TERM-SPECIFIC CODE STARTS HERE
    */

    /*
    ** Initialize various OS modules
    **
    ** Other modules (clock, SIO, syscall, etc.) are expected to
    ** install their own ISRs in their initialization routines.
    */

    __cio_puts( "System initialization starting.\n" );
    __cio_puts( "-------------------------------\n" );

    __cio_puts( "Modules:" );

    // call the module initialization functions, being
    // careful to follow any module precedence requirements
    //
    // classic order:  kmem; queue; everything else

    _km_init();     // MUST BE FIRST
#if TRACING_KMEM || TRACING_KMEM_FREE
    __delay(50);    // about 1.25 seconds
#endif

    _q_init();      // MUST BE SECOND
#if TRACING_QUEUE
    __delay(50);
#endif

    // other module initialization calls here
    _clk_init();
    _pcb_init();
#if TRACING_PCB
    __delay(50);
#endif
    _sched_init();
    _stk_init();
#if TRACING_STACK
    __delay(50);
#endif
    _sio_init();
    _sys_init();
#if TRACING_SYSCALLS || TRACING_SYSRETS
    __delay(50);
#endif

    __cio_puts( "\nModule initialization complete.\n" );
    __cio_puts( "-------------------------------\n" );

    // report our configuration options
    _kreport( true );

    __delay( 100 );  // about 2.5 seconds

    /*
    ** Other tasks typically performed here:
    **
    **  Enabling any I/O devices (e.g., SIO xmit/rcv)
    */
#if DO_AHCI
    _ahci_init();

    __delay(1000);
#endif

#if DO_PIO
    pio_init();

    __delay(500);
#endif
    /*
    ** Create the initial user process
    ** 
    ** This code is largely stolen from the spawn() implementation
    ** in syscalls.c; if that changes, this must also change.
    */

    // allocate the necessary data structures
    Pcb *new = _pcb_alloc();
    assert( new != NULL );

    new->stack = _stk_alloc();
    assert( new->stack != NULL );

    // fill in the PCB
    new->pid = new->ppid = PID_INIT;
    new->state = New;
    new->priority = System;

    // process context area and initial stack contents
    Context *ctx = _stk_setup( new->stack, 7, "init\r+" );
#if TRACING_SPAWN
    __cio_printf( "++ _kinit: init context %08x\n", (uint32_t) ctx );
    __delay( 100 );
#endif
    assert( ctx != NULL );

    // remember where the context area is
    new->context = ctx;

    // make sure the context is clean
    CLEAR_PTR( ctx );

    // critical register values
    ctx->eflags = DEFAULT_EFLAGS;
    ctx->esp = (uint32_t) (ctx + 1);    // not actually restored
    ctx->eip = (uint32_t) init;
    ctx->cs = GDT_CODE;
    ctx->ss = GDT_STACK;
    ctx->ds = ctx->es = ctx->fs = ctx->gs = GDT_DATA;

    // add to the process table (much easier here because
    // this is the only process in the system!)
    _activeProcs[0] = new;
    _nProcs = 1;
#if TRACING_SPAWN
    _ptable_dump( "++ ptable now:", true );
    __delay( 100 );
#endif

    // schedule and dispatch it
    _schedule( new );
#if TRACING_SPAWN
    __cio_puts( "++ scheduled....\n" );
    __delay( 100 );
#endif
    _dispatch();
#if TRACING_SPAWN
    __cio_puts( "++ dispatched....\n" );
    __delay( 100 );
#endif

    /*
    ** END OF TERM-SPECIFIC CODE
    **
    ** Finally, report that we're all done.
    */

    __cio_puts( "System initialization complete.\n" );
    __cio_puts( "-------------------------------\n" );

    _sio_enable( SIO_RX );
}

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
bool_t _grow_list( uint32_t elsize, void (*free)(void *) ) {

    // allocate a chunk of memory
    void *tmp = _km_slice_alloc();

    // if that failed, we're powerless to continue
    if( tmp == NULL ) {
        return false;
    }

    // got it; iterate through it, freeing each "thing"
    for( int i = 0; i < (SZ_SLICE/elsize); ++i ) {
        free( tmp );
        tmp += elsize;
    }

    return true;
}

/**
** _kpanic - kernel-level panic routine
**
** usage:  _kpanic( msg )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
**
** @param msg  String containing a relevant message to be printed if
**             it is non-NULL
*/
void _kpanic( char *msg ) {

    __cio_puts( "\n\n***** KERNEL PANIC *****\n\n" );
    __cio_printf( "Msg: %s\n", msg ? msg : "(none)" );

    // dump a bunch of potentially useful information

    // EXAMPLES

    // dump the contents of the current PCB
    _pcb_dump( "Current", _current );

    // dump the contents of the process table
    _ptable_dump( "Processes", false );

#if PANIC_DUMPS_QUEUES
    // dump the entire contents of the queues
    // _queue_dump( "Sleep queue", _sleepQueue );
    // etc.
#else
    // just dump the queue sizes
    // __cio_printf( "Queue sizes:  sleep %d", _q_length(_sleepQueue) );
    // etc.
#endif

    // could dump other stuff here, too

    // could do a stack trace for the kernel, or for the current process

   __panic( "KERNEL PANIC" );
}
