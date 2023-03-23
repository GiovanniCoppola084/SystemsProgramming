#ifndef SHELL_H_
#define SHELL_H_

#include "users.h"
#include "ulib.h"

// shorthand way to generate code to return the result from spawn()
#define DOIT(e,s) return( spawn(e,User,sizeof(s),s) )

static int run( char which ) {
    char buf[128];

    switch( which ) {
#ifdef SPAWN_A
    case 'A': DOIT( main1, "userA\rA\r30" ); break;
#endif
#ifdef SPAWN_B
    case 'B': DOIT( main1, "userB\rB\r30" ); break;
#endif
#ifdef SPAWN_C
    case 'C': DOIT( main1, "userC\rC\r30" ); break;
#endif
#ifdef SPAWN_D
    case 'D': DOIT( main2, "userD\rD\r20" ); break;
#endif
#ifdef SPAWN_E
    case 'E': DOIT( main2, "userE\rE\r20" ); break;
#endif
#ifdef SPAWN_F
    case 'F': DOIT( main3, "userF\rF\r20" ); break;
#endif
#ifdef SPAWN_G
    case 'G': DOIT( main3, "userG\rG\r10" ); break;
#endif
#ifdef SPAWN_H
    case 'H': DOIT( userH, "userH\rH\r4" ); break;
#endif
#ifdef SPAWN_I
    case 'I': DOIT( userI, "userI\rI" ); break;
#endif
#ifdef SPAWN_J
    case 'J': DOIT( userJ, "userJ\rJ" ); break;
#endif
#ifdef SPAWN_K
    case 'K': DOIT( main4, "userK\rK\r8" ); break;
#endif
#ifdef SPAWN_L
    case 'L': DOIT( main4, "userL\rL\r5" ); break;
#endif
#ifdef SPAWN_M
    case 'M': DOIT( main5, "userM\rM\rf\r5" ); break;
#endif
#ifdef SPAWN_N
    case 'N': DOIT( main5, "userN\rN\rt\r5" ); break;
#endif
#ifdef SPAWN_P
    case 'P': DOIT( userP, "userP\rP\r3\r2" ); break;
#endif
#ifdef SPAWN_Q
    case 'Q': DOIT( userQ, "userQ\rQ" ); break;
#endif
#ifdef SPAWN_R
    case 'R': DOIT( userR, "userR\rR\r10" ); break;
#endif
#ifdef SPAWN_S
    case 'S': DOIT( userS, "userS\rS\r20" ); break;
#endif
#ifdef SPAWN_T
    case 'T': DOIT( main6, "userT\rT\rw\r6" ); break;
#endif
#ifdef SPAWN_U
    case 'U': DOIT( main6, "userU\rU\rW\r6" ); break;
#endif
#ifdef SPAWN_V
    case 'V': DOIT( main6, "userV\rV\rk\r6" ); break;
#endif
    default:
        sprint( buf, "+++ Shell: unknown cmd '%c'\n", which );
        cwrites( buf );
    }

    // unlikely to be an actual error code
    return( -90210 );
};

static void report( void ) {
    char *cmds =
#ifdef SPAWN_A
    " A"
#endif
#ifdef SPAWN_B
    " B"
#endif
#ifdef SPAWN_C
    " C"
#endif
#ifdef SPAWN_D
    " D"
#endif
#ifdef SPAWN_E
    " E"
#endif
#ifdef SPAWN_F
    " F"
#endif
#ifdef SPAWN_G
    " G"
#endif
#ifdef SPAWN_H
    " H"
#endif
#ifdef SPAWN_I
    " I"
#endif
#ifdef SPAWN_J
    " J"
#endif
#ifdef SPAWN_K
    " K"
#endif
#ifdef SPAWN_L
    " L"
#endif
#ifdef SPAWN_M
    " M"
#endif
#ifdef SPAWN_N
    " N"
#endif
#ifdef SPAWN_P
    " P"
#endif
#ifdef SPAWN_Q
    " Q"
#endif
#ifdef SPAWN_R
    " R"
#endif
#ifdef SPAWN_S
    " S"
#endif
#ifdef SPAWN_T
    " T"
#endif
#ifdef SPAWN_U
    " U"
#endif
#ifdef SPAWN_V
    " V"
#endif
;

    swrites( "\nAvailable commands:\n" );
    swrites( " Spawn a process: " );
    swrites( cmds );
    swrites( "\n Help:  h\n Exit:  x\n" );
}

/**
** shell - extremely simple shell for spawning test programs
**
** Scheduled by _kshell() when the character 'u' is typed on
** the console keyboard.
*/
USERMAIN( shell ) {
    char buf[128];
    char ch = 'h';  // initial command

    // keep the compiler happy
    (void) arglen;
    (void) args;

    // register as the official user shell
    int whom = ushell();
    if( whom != 0 ) {
        // there was already one?
        cwrites( "+++ Shell couldn't register, one exists already\n" );
        exit( 1 );
    }

    // report that we're up and running
    cwrites( "+++ Shell is ready\n" );
    swritech( '@' );

 onceMore:

    cwrites( "+++ Shell is sleeping\n" );

    // the shell sleeps for up to a week; when the character 'u'
    // is typed on the console, the kernel wakes the shell up
    // and dispatches it
    //
    // the shell sleeps for one week; this way, it should always
    // be the last process in the sleep queue

    // one week is 604,800 seconds, or 604,800,000 clock ticks
    sleep( SEC_TO_MS(60*60*24*7) );

    // loop forever
    for(;;) {

        // process the current command
        if( ch == 'h' ) {

            // "help"
            report();

        } else if( ch == 'x' ) {

            // "exit"
            goto onceMore;

        } else {

            // find the command character
            int pid = run( ch );

            if( pid == -90210 ) {

                // bad command
                report();

            } else if( pid < 1 ) {

                // spawn() failed
                sprint( buf, "+++ Shell spawn %c failed, code %d\n", ch, pid );
                cwrites( buf );

            } else {

                // wait for the process to terminate
                int32_t status;
                whom = waitpid( pid, &status );

                // figure out the result
                if( whom != pid ) {
                    sprint( buf, "shell: waitpid() returned %d\n", whom );
                } else {
                    sprint( buf, "shell: PID %d exit status %d\n",
                            whom, status );
                }
                // report it
                swrites( buf );
            }

        }

        // prompt for the next command
        swrites( "\n? " );
        int n = read( CHAN_SIO, buf, 1 );
        if( n < 1 ) {
            sprint( buf, "+++ Shell input error, code %d\n", n );
            cwrites( buf );
            goto onceMore;
        }

    }

    cwrites( "!!! shell exited loop???\n" );
    exit( 1 );
}

#endif
