#ifndef USER_P_H_
#define USER_P_H_

#include "users.h"
#include "ulib.h"

/**
** User function P:  exit, sleep, write
**
** Reports itself, then loops reporting itself
**
** Invoked as:  userP  x  [ n  [ t ] ]
**   where x is the ID character
**         n is the iteration count (defaults to 3)
**         t is the sleep time (defaults to 2 seconds)
*/

USERMAIN( userP ) {
    int count = 3;    // default iteration count
    char ch = 'p';    // default character to print
    int nap = 2;      // nap time

    // process the command-line arguments
    ARG_PROC( 0, args, 5, nargs, "userP" );
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

    for( int i = 0; i < count; ++i ) {
        sleep( SEC_TO_MS(nap) );
        write( CHAN_SIO, &ch, 1 );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
