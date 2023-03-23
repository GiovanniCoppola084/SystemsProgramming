#ifndef USER_R_H_
#define USER_R_H_

#include "users.h"
#include "ulib.h"

/**
** User function R:  exit, sleep, read, write
**
** Reports itself, then loops forever reading and printing SIO characters
**
** Invoked as:  userR  x  [ s ]
**   where x is the ID character
**         s is the initial delay time (defaults to 10)
*/

USERMAIN( userR ) {
    char ch = 'r';    // default character to print
    int delay = 10;   // initial delay
    char buf[128];
    char b2[8];

    // process the command-line arguments
    ARG_PROC( 0, args, 5, nargs, "userR" );
    if( nargs > 1 ) {
        ch = argv[1][0];
        if( nargs > 2 ) {
            delay = str2int( argv[3], 10 );
        }
    }

    // announce our presence
    b2[0] = ch;
    b2[1] = '[';  // just in case!
    b2[2] = '?';
    b2[3] = ']';
    b2[4] = '\0';

    write( CHAN_SIO, b2, 1 );

    sleep( SEC_TO_MS(delay) );

    for(;;) {
        int n = read( CHAN_SIO, &b2[2], 1 );
        if( n != 1 ) {
            sprint( buf, "!! %c, read returned %d\n", ch, n );
            cwrites( buf );
            if( n == -1 ) {
                // wait a bit
                sleep( SEC_TO_MS(1) );
            }
        } else {
	    write( CHAN_SIO, b2, 4 );
        }
    }

    sprint( buf, "!! %c left its infinite loop?!?!?!?\n", ch );
    cwrites( buf );
    exit( 1 );

    return( 42 );  // shut the compiler up!

}

#endif
