/**
** File:	users.c
**
** Author:	numerous CSCI-452 classes
**
** Contributor:
**
** Description:	User-level code.
*/

#include "common.h"
#include "users.h"

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS; see the users.h
** header file for a summary of which system calls are tested by
** each user function.
**
** Output from user processes is usually alphabetic.  Uppercase
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the users.h header file.
*/

/*
** Prototypes for all user main routines (even ones that may not exist,
** for completeness)
*/

int32_t idle( uint32_t, void * );  int32_t shell( uint32_t, void * );

int32_t main1( uint32_t, void * ); int32_t main2( uint32_t, void * );
int32_t main3( uint32_t, void * ); int32_t main4( uint32_t, void * );
int32_t main5( uint32_t, void * ); int32_t main6( uint32_t, void * );

int32_t userA( uint32_t, void * ); int32_t userB( uint32_t, void * );
int32_t userC( uint32_t, void * ); int32_t userD( uint32_t, void * );
int32_t userE( uint32_t, void * ); int32_t userF( uint32_t, void * );
int32_t userG( uint32_t, void * ); int32_t userH( uint32_t, void * );
int32_t userI( uint32_t, void * ); int32_t userJ( uint32_t, void * );
int32_t userK( uint32_t, void * ); int32_t userL( uint32_t, void * );
int32_t userM( uint32_t, void * ); int32_t userN( uint32_t, void * );
int32_t userO( uint32_t, void * ); int32_t userP( uint32_t, void * );
int32_t userQ( uint32_t, void * ); int32_t userR( uint32_t, void * );
int32_t userS( uint32_t, void * ); int32_t userT( uint32_t, void * );
int32_t userU( uint32_t, void * ); int32_t userV( uint32_t, void * );
int32_t userW( uint32_t, void * ); int32_t userX( uint32_t, void * );
int32_t userY( uint32_t, void * ); int32_t userZ( uint32_t, void * );

/*
** The user processes
**
** We #include the source code from the userland/ directory only if
** a specific process is being spawned.
**
** Remember to #include the code required by any process that will
** be spawned - e.g., userH spawns userZ.  The user code files all
** contain CPP include guards, so multiple inclusion of a source
** file shouldn't cause problems.
*/

#if defined(SPAWN_A) || defined(SPAWN_B) || defined(SPAWN_C)
#include "userland/main1.c"
#endif

#if defined(SPAWN_D) || defined(SPAWN_E)
#include "userland/main2.c"
#endif

#if defined(SPAWN_F) || defined(SPAWN_G)
#include "userland/main3.c"
#endif

#if defined(SPAWN_H)
#include "userland/userH.c"
#include "userland/userZ.c"
#endif

#if defined(SPAWN_I)
#include "userland/userI.c"
#include "userland/userW.c"
#endif

#if defined(SPAWN_J)
#include "userland/userJ.c"
#include "userland/userY.c"
#endif

#if defined(SPAWN_K) || defined(SPAWN_L)
#include "userland/main4.c"
#include "userland/userX.c"
#endif

#if defined(SPAWN_M) || defined(SPAWN_N)
#include "userland/main5.c"
#include "userland/userW.c"
#include "userland/userZ.c"
#endif

#if defined(SPAWN_P)
#include "userland/userP.c"
#endif

#if defined(SPAWN_Q)
#include "userland/userQ.c"
#endif

#if defined(SPAWN_R)
#include "userland/userR.c"
#endif

#if defined(SPAWN_S)
#include "userland/userS.c"
#endif

#if defined(SPAWN_T) || defined(SPAWN_U) || defined(SPAWN_V)
#include "userland/main6.c"
#include "userland/userW.c"
#endif

/*
** Simple user-level shell (if being used)
*/

#if defined(SPAWN_SHELL)
#include "userland/shell.c"
#endif

/*
** System processes - these should always be included here
*/

#include "userland/init.c"

#include "userland/idle.c"
