#ifndef MAIN_4_H_
#define MAIN_4_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #4:  exit, spawn, sleep, write
**
** Loops, spawning N copies of userX and sleeping between spawns.
**
** Invoked as:  main4  x  n
**   where x is the ID character
**         n is the iteration count (defaults to 5)
*/

USERMAIN( main4 ) {
    int count = 5;          // default iteration count
    char ch = '4';          // default character to print
    int nap = 30;           // nap time
    char msg2[] = "*4*";    // "error" message to print
    char buf[32];

    // process the command-line arguments
    ARG_PROC( 3, args, 5, nargs, "main4" );
    if( nargs == 3 ) {
        ch = argv[1][0];
        count = str2int( argv[2], 10 );
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    for( int i = 0; i < count ; ++i ) {

        write( CHAN_SIO, &ch, 1 );

        // second argument to X is 100 plus the iteration number
        sprint( buf, "userX\rX\r%d", 100 + i );
        int whom = spawn( userX, User, strlen(buf)+1, buf );
        if( whom < 0 ) {
            swrites( msg2 );
        } else {
            write( CHAN_SIO, &ch, 1 );
        }

        sleep( SEC_TO_MS(nap) );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
