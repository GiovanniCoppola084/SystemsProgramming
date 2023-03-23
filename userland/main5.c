#ifndef MAIN_5_H_
#define MAIN_5_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #5:  exit, spawn, write
**
** Iterates spawning copies of userW (and possibly userZ), reporting
** their PIDs as it goes.
**
** Invoked as:  main5  b  n
**   where x is the ID character
**         b is the w&z boolean
**         n is the iteration count
*/

USERMAIN( main5 ) {
    int count = 5;  // default iteration count
    char ch = '5';  // default character to print
    int alsoZ = 0;  // also do userZ?
    char msgw[] = "*5w*";
    char msgz[] = "*5z*";

    // process the command-line arguments
    ARG_PROC( 3, args, 5, nargs, "main5" );
    if( nargs == 4 ) {
        ch = argv[1][0];
        alsoZ = argv[2][0] == 't';
        count = str2int( argv[3], 10 );
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the argument vector(s)

    // W:  15 iterations, 5-second sleep
    char *argsw = "userW\rW\r15\r5";
    int wlen = strlen( argsw ) + 1;

    // Z:  15 iterations
    char *argsz = "userZ\rZ\r15";
    int zlen = strlen( argsz ) + 1;

    for( int i = 0; i < count; ++i ) {
        write( CHAN_SIO, &ch, 1 );
        int whom = spawn( userW, User, wlen, argsw  );
        if( whom < 1 ) {
            swrites( msgw );
        }
        if( alsoZ ) {
            whom = spawn( userZ, User, zlen, argsz );
            if( whom < 1 ) {
                swrites( msgz );
            }
        }
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
