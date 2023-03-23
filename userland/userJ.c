#ifndef USER_J_H_
#define USER_J_H_

#include "users.h"
#include "ulib.h"

/**
** User function J:  exit, spawn, write
**
** Reports, tries to spawn lots of children, then exits
**
** Invoked as:  userJ  x  [ n ]
**   where x is the ID character
**         n is the number of children to spawn (defaults to 2 * N_PROCS)
*/

USERMAIN( userJ ) {
    int count = 2 * MAX_PROCS;  // number of children to spawn
    char ch = 'J';              // default character to print

    // process the command-line arguments
    ARG_PROC( 0, args, 5, nargs, "userJ" );
    if( nargs > 1 ) {
        ch = argv[1][0];
        if( nargs > 2 ) {
            count = str2int( argv[2], 10 );
        }
    }

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the command-line arguments
    char *argsy = "userY\rY\r10";
    int ylen = strlen( argsy ) + 1;

    for( int i = 0; i < count ; ++i ) {
        int whom = spawn( userY, User, ylen, argsy );
        if( whom < 0 ) {
            write( CHAN_SIO, "!j!", 3 );
        } else {
            write( CHAN_SIO, &ch, 1 );
        }
    }

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
