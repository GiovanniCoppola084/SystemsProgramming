#ifndef MAIN_6_H_
#define MAIN_6_H_

#include "users.h"
#include "ulib.h"

/**
** User function main #6: exit, spawn, kill, waitpid, sleep, write
**
** Reports, then loops spawing userW, sleeps, then waits for or kills
** all its children.
**
** Invoked as:  main6  x  b  c
**   where x is the ID character
**         b is wait/kill indicator ('w', 'W', or 'k')
**         c is the child count
*/

#ifndef MAX_CHILDREN
#define MAX_CHILDREN    50
#endif

USERMAIN( main6 ) {
    int count = 3;    // default child count
    char ch = '6';    // default character to print
    int nap = 8;      // nap time
    char what = 1;    // wait or kill?  default is wait
    char buf[128];
    pid_t children[MAX_CHILDREN];
    int nkids = 0;

    // process the command-line arguments
    ARG_PROC( 4, args, 5, nargs, "main6" );
    if( nargs == 4 ) {
        ch = argv[1][0];
        what = argv[2][0] == 'w';   // 'w' -> wait, else -> kill
        count = str2int( argv[3], 10 );
    }

    // secondary output (for indicating errors)
    char ch2[] = "*?*";
    ch2[1] = ch;

    // announce our presence
    write( CHAN_SIO, &ch, 1 );

    // set up the argument vector
    char *argsw = "userW\rW\r10\r5";
    int wlen = strlen( argsw ) + 1;

    for( int i = 0; i < count; ++i ) {
        int whom = spawn( userW, User, wlen, argsw );
        if( whom < 0 ) {
            swrites( ch2 );
        } else {
            children[nkids++] = whom;
        }
    }

    // let the children start
    sleep( SEC_TO_MS(nap) );

    // collect exit status information

    int n = 0;

    do {
        pid_t this;
        int32_t status;

        // are we waiting for or killing it?
        if( what ) {
            this = waitpid( children[n], &status );
        } else {
            this = kill( children[n] );
        }

        // what was the result?
        if( this < E_SUCCESS ) {

            // uh-oh - something went wrong

            // "no children" means we're all done
            if( this != E_NO_CHILDREN ) {
                if( what ) {
                    sprint( buf, "!! %c: waitpid() status %d\n", ch, this );
                } else {
                    sprint( buf, "!! %c: kill() status %d\n", ch, this );
                }
            } else {
                sprint( buf, "!! %c: no children\n", ch );
            }

            // regardless, we're outta here
            break;

        } else {

            // locate the child
            int i;
            for( i = 0; i < nkids; ++i ) {
                if( children[i] == this ) {
                    break;
                }
            }
            if( i < nkids ) {
                if( i != n ) {
                    sprint( buf, "== %c: child %d (%d,%d) status %d\n",
                            ch, i, n, this, status );
                } else {
                    sprint( buf, "== %c: child %d (%d) status %d\n",
                            ch, i, this, status );
                }
            } else {
                sprint( buf, "!! %c: child PID %d term, NOT FOUND\n",
                        ch, this );
            }
        }

        cwrites( buf );

        ++n;

    } while( n < nkids );

    exit( 0 );

    return( 42 );  // shut the compiler up!
}

#endif
