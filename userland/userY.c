#ifndef USER_Y_H_
#define USER_Y_H_

#include "users.h"
#include "ulib.h"

/**
** User function Y:  exit, sleep, write
**
** Reports its PID, then iterates N times printing 'Yx' and
** sleeping for one second, then exits.
**
** Invoked as:  userY  x  [ n ]
**   where x is the ID character
**         n is the iteration count (defaults to 10)
*/

USERMAIN( userY ) {
    int count = 10;   // default iteration count
    char ch = 'Y';    // default character to print
    char ch2;         // secondary character
    char buf[128];

    // process the command-line arguments
    ARG_PROC( 0, args, 5, nargs, "userY" );
    if( nargs > 1 ) {
        ch2 = argv[1][0];
        if( nargs > 2 ) {
            count = str2int( argv[2], 10 );
        }
    }

    sprint( buf, " %c[%c] ", ch, ch2 );

    for( int i = 0; i < count ; ++i ) {
        swrites( buf );
        DELAY(STD);
        sleep( SEC_TO_MS(1) );
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
