#ifndef USER_H_H_
#define USER_H_H_

#include "users.h"
#include "ulib.h"

/**
** User function H:  exit, fork, exec, sleep, write
**
** Prints its ID, then spawns 'n' children; exits before they terminate.
**
** Invoked as:  userH  x  n
**   where x is the ID character
**         n is the number of children to spawn
*/

USERMAIN( userH ) {
    int32_t ret = 0;  // return value
    int count = 5;    // child count
    char ch = 'H';    // default character to print
    char buf[128];
    int whom;

    // process the argument(s)
    ARG_PROC( 3, args, 5, nargs, "userH" );
    if( nargs == 3 ) {
        ch = argv[1][0];
        count = str2int( argv[2], 10 );
    }

    // announce our presence
    swritech( ch );

    // we spawn user Z and then exit before it can terminate
    // userZ 'Z' 10

    char *argsz = "userZ\rZ\r10";
    int zlen = strlen( argsz ) + 1;

    for( int i = 0; i < count; ++i ) {

        // spawn a child
        whom = spawn( userZ, User, zlen, argsz );

        // our exit status is the number of failed spawn() calls
        if( whom < 0 ) {
            sprint( buf, "!! %c spawn() failed, returned %d\n", ch, whom );
            cwrites( buf );
            ret += 1;
        }
    }

    // yield the CPU so that our child(ren) can run
    sleep( 0 );

    // announce our departure
    swritech( ch );

    exit( ret );

    return( 42 );  // shut the compiler up!
}

#endif
