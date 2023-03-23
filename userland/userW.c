#ifndef USER_W_H_
#define USER_W_H_

#include "users.h"
#include "ulib.h"

/**
** User function W:  exit, sleep, write
**
** Reports its presence, then iterates 'n' times printing identifying
** information and sleeping, before exiting.
**
** Invoked as:  userW  x  [ n  [ t ] ]
**   where x is the ID character
**         n is the iteration count (defaults to 20)
**         t is the sleep time (defaults to 3 seconds)
*/

USERMAIN( userW ) {
    int count = 20;   // default iteration count
    char ch = 'w';    // default character to print
    int nap = 3;      // nap length
    char buf[128];

    // process the command-line arguments
    ARG_PROC( 0, args, 5, nargs, "userW" );
    if( nargs > 1 ) {
        ch = argv[1][0];
        if( nargs > 2 ) {
            count = str2int( argv[2], 10 );
            if( nargs > 3 ) {
                nap = str2int( argv[3], 10 );
            }
        }
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    for( int i = 0; i < count ; ++i ) {
        sprint( buf, " %c[%d] ", ch, i );
        swrites( buf );
        sleep( SEC_TO_MS(nap) );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
