#ifndef IDLE_H_
#define IDLE_H_

#include "users.h"
#include "ulib.h"

/**
** Idle process:  write, getpid, gettime, exit
**
** Reports itself, then loops forever delaying and printing a character.
**
** Invoked as:  idle
*/

USERMAIN( idle ) {
    char ch = '.';

    // ignore the command-line arguments
    (void) arglen;
    (void) args;
    
    // report our presence on the console
    cwrites( "Idle started\n" );

    write( CHAN_SIO, &ch, 1 );

    // idle() should never block - it must always be available
    // for dispatching when we need to pick a new current process

    for(;;) {
        DELAY(LONG);
        write( CHAN_SIO, &ch, 1 );
    }

    // we should never reach this point!

    cwrites( "+++ IDLE EXITING !?!?!\n" );

    exit( 1 );

    return( 42 );
}

#endif
