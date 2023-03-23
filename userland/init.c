#ifndef INIT_H_
#define INIT_H_

#include "users.h"
#include "ulib.h"

/**
** Initial process; it starts the other top-level user processes.
**
** Prints a message at startup, '+' after each user process is spawned,
** and '!' before transitioning to wait() mode to the SIO, and
** startup and transition messages to the console.  It also reports
** each child process it collects via wait() to the console along
** with that child's exit status.
*/

/*
** For the test programs in the baseline system, command-line arguments
** follow these rules.  The first (up to) three entries will be strings
** containing the following things:
**
**      argv[0] the name used to "invoke" this process
**      argv[1] the "character to print" (identifies the process)
**      argv[2] either an iteration count or a sleep time
**
** These are 'encoded' as a C string literal with carriage returns as
** the argument separators; e.g., if a program is being run as 'userA',
** prints the character 'A', and iterates 30 times, the arg string
** would be
**
**      "userA\rA\r30"
**
** The ARG_PROC macro (in users.h) will parse this into a traditional
** argv array containing "userA", "A", and "30", followed by a NULL
** pointer, using the parseArgString() library function.
**
** See the comment at the beginning of each user-code source file for
** information on the argument list that code expects.
*/

/*
** "Spawn table" process entry.
*/
typedef struct proc_s {
    int32_t (*entry)( uint32_t, void *);    // entry point
    uint32_t prio;                          // process priority
    uint32_t len;                           // argument buffer length
    const void *buf;                        // argument buffer
} Proc;

/*
** Create a spawn table entry for a process with a  string literal
** as its argument buffer.  This relies on the fact that sizeof()
** applied to a string literal produces a byte count that includes
** the trailing NUL character - e.g., sizeof("abc") is 4.
*/
#define PROCENT(e,p,a) { e, p, sizeof(a), (void *) a }

/*
** The spawn table.  Each entry represents one process that is to
** be started by init().
*/
Proc spawnTable[] = {

    // the idle process; it runs at Deferred priority,
    // so it will only be dispatched when there is
    // nothing else available to be dispatched
    PROCENT( idle, Deferred, "idle\r." ),

#ifdef SPAWN_SHELL
    // spawn a "test shell" process; it runs at System
    // priority, so it takes precedence over all other
    // user-level processes when it's not sleeping
    PROCENT( shell, System, "shell" ),
#endif

    // Users A-C each run main1, which loops printing its character
#ifdef SPAWN_A
    PROCENT( main1, User, "userA\rA\r30" ),
#endif
#ifdef SPAWN_B
    PROCENT( main1, User, "userB\rB\r30" ),
#endif
#ifdef SPAWN_C
    PROCENT( main1, User, "userC\rC\r30" ),
#endif

    // Users D and E run main2, which is like main1 but doesn't exit()
#ifdef SPAWN_D
    PROCENT( main2, User, "userD\rD\r20" ),
#endif
#ifdef SPAWN_E
    PROCENT( main2, User, "userE\rE\r20" ),
#endif

    // Users F and G run main3, which sleeps between write() calls
#ifdef SPAWN_F
    PROCENT( main3, User, "userF\rF\r20" ),
#endif
#ifdef SPAWN_G
    PROCENT( main3, User, "userG\rG\r10" ),
#endif

    // User H tests reparenting of orphaned children
#ifdef SPAWN_H
    PROCENT( userH, User, "userH\rH\r4" ),
#endif

    // User I spawns several children, kills one, and waits for all
#ifdef SPAWN_I
    PROCENT( userI, User, "userI\rI" ),
#endif

    // User J tries to spawn 2 * N_PROCS children
#ifdef SPAWN_J
    PROCENT( userJ, User, "userJ\rJ" ),
#endif

    // Users K and L iterate spawning userX and sleeping
#ifdef SPAWN_K
    PROCENT( main4, User, "userK\rK\r8" ),
#endif
#ifdef SPAWN_L
    PROCENT( main4, User, "userL\rL\r5" ),
#endif

    // Users M and N spawn copies of userW and userZ via main5
#ifdef SPAWN_M
    PROCENT( main5, User, "userM\rM\rf\r5" ),
#endif
#ifdef SPAWN_N
    PROCENT( main5, User, "userN\rN\rt\r5" ),
#endif

    // There is no user O

    // User P iterates, reporting system time and stats, and sleeping
#ifdef SPAWN_P
    PROCENT( userP, User, "userP\rP\r3\r2" ),
#endif

    // User Q tries to execute a bad system call
#ifdef SPAWN_Q
    PROCENT( userQ, User, "userQ\rQ" ),
#endif

    // User R reads from the SIO one character at a time, forever
#ifdef SPAWN_R
    PROCENT( userR, User, "userR\rR\r10" ),
#endif

    // User S loops forever, sleeping on each iteration
#ifdef SPAWN_S
    PROCENT( userS, User, "userS\rS\r20" ),
#endif

    // Users T-V run main6(); they spawn copies of userW
    //   User T waits for any child
    //   User U waits for each child by PID
    //   User V kills each child
#ifdef SPAWN_T
    PROCENT( main6, User, "userT\rT\rw\r6" ),
#endif
#ifdef SPAWN_U
    PROCENT( main6, User, "userU\rU\rW\r6" ),
#endif
#ifdef SPAWN_V
    PROCENT( main6, User, "userV\rV\rk\r6" ),
#endif
    
    // a dummy entry to use as a sentinel
    { 0, 0, 0, 0 }
};

USERMAIN( init ) {
    char ch;
    static int invoked = 0;
    char buf[128];

    if( invoked > 0 ) {
        cwrites( "*** INIT RESTARTED??? ***\n" );
        for(;;);
    }

    cwrites( "Init started\n" );
    ++invoked;

    // process the command line
    ARG_PROC( 2, args, 5, nargs, "init" );

    if( argv[1] == NULL ) {
        ch = '+';
    } else {
        ch = argv[1][0];
    }

    // home up, clear on a TVI 925
    swritech( '\x1a' );

    // wait a bit
    DELAY(SHORT);

    // a bit of Dante to set the mood
    swrites( "\n\nSpem relinquunt qui huc intrasti!\n\n\r" );

    /*
    ** Start all the user processes
    */

    cwrites( "INIT: starting user processes\n" );

    int whom = 0;
    Proc *next = spawnTable;

    while( next->entry != NULL ) {

        // kick off the process
        whom = spawn( next->entry, next->prio, next->len, next->buf );
        if( whom < 1 ) {
            // error!
            sprint( buf, "INIT: spawn 0x%08x failed, code %d\n",
                    (uint32_t) (next->entry), whom );
            cwrites( buf );
        } else {
            swritech( ch );
        }

        ++next;
    }

    swrites( " !!!\r\n\n" );

    /*
    ** At this point, we go into an infinite loop waiting
    ** for our children (direct, or inherited) to exit.
    */

    cwrites( "INIT: transitioning to wait() mode\n" );

    for(;;) {
        int32_t status;
        int whom = waitpid( 0, &status );

        // PIDs must be positive numbers!
        if( whom <= 0 ) {
            // cwrites( "INIT: wait() says 'no children'???\n" );
        } else {
            sprint( buf, "INIT: %d exit(%d)\n", whom, status );
            cwrites( buf );
        }
    }

    /*
    ** SHOULD NEVER REACH HERE
    */

    cwrites( "*** INIT IS EXITING???\n" );
    exit( 1 );

    return( 0 );  // shut the compiler up!
}

#endif
