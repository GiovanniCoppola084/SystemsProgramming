#ifndef USER_X_H_
#define USER_X_H_

#include "users.h"
#include "ulib.h"

/**
** User function X:  exit, write
**
** Prints its PID at start and exit, iterates printing its character
** N times, and exits with a status of 12.
**
** Invoked as:  userX  x  n
**   where x is the ID character
**         n is a value to be used when printing our character
*/

USERMAIN( userX ) {
    int count = 20;   // iteration count
    char ch = 'x';    // default character to print
    int value = 17;   // default value
    char buf[128];

    // process the command-line arguments
    ARG_PROC( 3, args, 5, nargs, "userX" );
    if( nargs == 3 ) {
        ch = argv[1][0];
        value = str2int( argv[2], 10 );
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    sprint( buf, " %c[%d] ", ch, value );

    for( int i = 0; i < count ; ++i ) {
        swrites( buf );
        DELAY(STD);
    }

    exit( 12 );

    return( 42 );  // shut the compiler up!
}

#endif
