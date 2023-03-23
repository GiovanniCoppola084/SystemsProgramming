/**
** @file clock.c
**
** @author CSCI-452 class of 20225
**
** Clock module  implementation
*/

#define SP_KERNEL_SRC

#include "x86arch.h"
#include "x86pic.h"
#include "x86pit.h"

#include "common.h"

#include "clock.h"
#include "kernel.h"
#include "processes.h"
#include "queues.h"
#include "sched.h"
#include "sio.h"
#include "syscalls.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// pinwheel control variables
static uint32_t _pinwheel;   // pinwheel counter
static uint32_t _pindex;     // index into pinwheel string

/*
** PUBLIC GLOBAL VARIABLES
*/

// current system time
time_t _systemTime;

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  _clk_isr
**
** The ISR for the clock
**
** @param vector    Vector number for the clock interrupt
** @param code      Error code (0 for this interrupt)
*/
static void _clk_isr( int vector, int code ) {

    // spin the pinwheel

    ++_pinwheel;
    if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
        _pinwheel = 0;
        ++_pindex;
        __cio_putchar_at( 0, 0, "|/-\\"[ _pindex & 3 ] );
    }

#if defined(STATUS)
    // Periodically, dump the queue lengths and the SIO status (along
    // with the SIO buffers, if non-empty).
    //
    // Define the symbol STATUS with a value equal to the desired
    // reporting frequency, in seconds.

    uint32_t counts[ N_STATES ];

    if( (_systemTime % SEC_TO_TICKS(STATUS)) == 0 ) {
        (void) _pcount( counts );
        __cio_printf_at( 2, 0,
#if defined(USER_SHELL)
"procs: n/%d r/%d R/%d s/%d w/%d b/%d z/%d k/%d US RQ[%d,%d,%d] SL[%d] SI[%d]",
#else
"procs: n/%d r/%d R/%d s/%d w/%d b/%d z/%d k/%d    RQ[%d,%d,%d] SL[%d] SI[%d]",
#endif
            counts[New],      counts[Ready],   counts[Running],
            counts[Sleeping], counts[Waiting], counts[Blocked],
            counts[Zombie],   counts[Killed],
            _readyLength[System], _readyLength[User], _readyLength[Deferred],
            _q_length( _sleepQueue ), _q_length( _sioReadQueue )
        );
        // _sio_dump( true );
        // _ptable_dump( "Ptbl", false );
    }
#endif

    // time marches on!
    ++_systemTime;

    // wake up any sleeping processes whose time has come
    //
    // we give them preference over the current process
    // (when it is scheduled again)

    do {

        // peek at the first member of the queue
        Payload *p = _q_peek( _sleepQueue );

        // the key value is the earliest wakeup time for any process
        // on the queue; if that's greater than the current time, or
        // if the queue is empty (indicated by a key value of 0),
        // there's nobody to awaken

        if( p == NULL || p->key == 0 || p->key > _systemTime ) {
            break;
        }

        // ok, we need to wake someone up
        Payload p2;
        assert1( _q_remove(_sleepQueue,&p2) );

        // schedule this process
        _schedule( (Pcb *) (p2.data) );

    } while( 1 );

    // check the current process to see if its time slice has expired
    _current->quantum -= 1;

    if( _current->quantum < 1 ) {
        // yes!  put it back on the ready queue
        _schedule( _current );
        // pick a new "current" process
        _dispatch();
    }

    // tell the PIC we're done
    __outb( PIC_PRI_CMD_PORT, PIC_EOI );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _clk_init
**
** Initializes the clock module
**
*/
void _clk_init( void ) {

    __cio_puts( " Clock" );

    // start the pinwheel
    _pinwheel = (CLOCK_FREQUENCY / 10) - 1;
    _pindex = 0;

    // return to the dawn of time
    _systemTime = 0;

    // configure the clock
    uint32_t divisor = PIT_FREQUENCY / CLOCK_FREQUENCY;
    __outb( PIT_CONTROL_PORT, PIT_0_LOAD | PIT_0_SQUARE );
    __outb( PIT_0_PORT, divisor & 0xff );        // LSB of divisor
    __outb( PIT_0_PORT, (divisor >> 8) & 0xff ); // MSB of divisor

    // register the second-stage ISR
    __install_isr( INT_VEC_TIMER, _clk_isr );
}
