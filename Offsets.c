/*
** File:    Offsets.c
**
** Author:      Warren R. Carithers
**
** Description:     Print byte offsets for fields in various structures.
**
** This program exists to simplify life.  If/when fields in a structure are
** changed, this can be modified, recompiled and executed to come up with
** byte offsets for use in accessing structure fields from assembly language.
** It makes use of the C 'offsetof' macro (defined since C89).
**
** IMPORTANT NOTE:  compiling this on a 64-bit architecture will yield
** incorrect results by default, as 64-bit GCC versions most often use
** the LP64 model (longs and pointers are 64 bits).  Add the "-mx32"
** option to the compiler (compile for x86_64, but use 32-bit sizes),
** and make sure you have the 'libc6-dev-i386' package installed (for
** Ubuntu systems).
**
** If invoked with the -h option, generates a header file named offsets.h
** which contains CPP macros for type sizes and fields offsets; otherwise,
** prints the same information to stdout.
*/

// make sure we get all the kernel stuff
#define SP_KERNEL_SRC


// include any OS headers that define data structures to be described
#include "common.h"
#include "processes.h"
#include "stacks.h"
#include "queues.h"
#include "vector.h"

// avoid complaints about NULL from stdio.h
#ifdef NULL
#undef NULL
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
** We don't include <time.h> because of conflicts with
** our own time_t type; instead, we use __time_t and
** provide our own prototypes for ctime() and time().
*/

extern char *ctime( const __time_t *timep );
extern __time_t time( __time_t *tloc );

/*
** Header comment, including header guard
**
** No newline on the "Creation date" line, because ctime()
** puts a newline at the end of the string it produces.
*/
char h_prefix[] = "/**\n"
"** @file offsets.h\n"
"**\n"
"** GENERATED AUTOMATICALLY - DO NOT EDIT\n"
"**\n"
"** Creation date: %s"
"**\n"
"** This header file contains C Preprocessor macros which expand\n"
"** into the byte offsets needed to reach fields within structs\n"
"** used in the baseline system.  Should those struct declarations\n"
"** change, the Offsets program should be modified (if needed),\n"
"** recompiled, and re-run to recreate this file.\n"
"*/\n"
"\n"
"#ifndef OFFSETS_H_\n"
"#define OFFSETS_H_\n";

/*
** Header guard suffix
*/
char h_suffix[] = "\n"
"#endif\n";

// are we generating the .h file?
int genheader = 0;

// header file stream
FILE *hfile;

// prefix for header file lines

// produce a report line
void process( const char *sname, const char *field, size_t bytes ) {
    if( genheader ) {
        char name[64];
        sprintf( name, "%s_%s", sname, field );
        fprintf( hfile, "#define\t%-23s\t%u\n", name, bytes );
    } else {
        printf( "  %-10s  %u\n", field, bytes );
    }
}

// create the .h file and dump out the header
void setheader( void ) {
    // trigger output into the header file
    genheader = 1;

    hfile = fopen( "offsets.h", "w" );
    if( hfile == NULL ) {
        perror( "offsets.h" );
        exit( 1 );
    }

    __time_t t;
    (void) time( &t );

    fprintf( hfile, h_prefix, ctime(&t) );
}

// introduce an "offsets" section
void hsection( const char *name, const char *typename, size_t size ) {
    if( genheader ) {
        fprintf( hfile, "\n// %s structure\n\n", typename );
        process( "SZ", name, size );
        fputc( '\n', hfile );
    } else {
        printf( "Offsets into %s (%u bytes):\n", typename, size );
    }
}

// introduce a "sizes" section
void tsection( const char *name, const char *typename ) {
    if( genheader ) {
        fprintf( hfile, "\n// Sizes of %s types\n\n", typename );
    } else {
        printf( "Sizes of %s types:\n", typename );
    }
}

int main( int argc, char *argv[] ) {

    // -h:  produce an offsets.h header file
    if( argc > 1 && strcmp(argv[1],"-h") == 0 ) {
        setheader();
    }

    /*
    ** Basic and simple/opaque types
    */

    tsection( "SZ", "basic" );
    process( "SZ", "char", sizeof(char) );
    process( "SZ", "short", sizeof(short) );
    process( "SZ", "int", sizeof(int) );
    process( "SZ", "long", sizeof(long) );
    process( "SZ", "long_long", sizeof(long long) );
    process( "SZ", "pointer", sizeof(void *) );
    fputc( '\n', genheader ? hfile : stdout );

    tsection( "SZ", "our" );
    process( "SZ", "int8_t", sizeof(int8_t) );
    process( "SZ", "int16_t", sizeof(int16_t) );
    process( "SZ", "int32_t", sizeof(int32_t) );
    process( "SZ", "int64_t", sizeof(int64_t) );
    process( "SZ", "uint8_t", sizeof(uint8_t) );
    process( "SZ", "uint16_t", sizeof(uint16_t) );
    process( "SZ", "uint32_t", sizeof(uint32_t) );
    process( "SZ", "uint64_t", sizeof(uint64_t) );
    process( "SZ", "bool_t", sizeof(bool_t) );
    process( "SZ", "pid_t", sizeof(pid_t) );
    process( "SZ", "state_t", sizeof(state_t) );
    process( "SZ", "time_t", sizeof(time_t) );
    process( "SZ", "state_t", sizeof(state_t) );
    process( "SZ", "prio_t", sizeof(prio_t) );
    process( "SZ", "key_t", sizeof(key_t) );
    process( "SZ", "Queue", sizeof(Queue) );
    process( "SZ", "Stack", sizeof(Stack) );
    process( "SZ", "ApplyFcn", sizeof(ApplyFcn) );
    process( "SZ", "LocateFcn", sizeof(LocateFcn) );
    process( "SZ", "OrderFcn", sizeof(OrderFcn) );
    fputc( '\n', genheader ? hfile : stdout );

    /*
    ** Structured types whose fields we are describing
    */

    hsection( "CTX", "Context", sizeof(Context) );
    process( "CTX", "ss", offsetof(Context,ss) );
    process( "CTX", "gs", offsetof(Context,gs) );
    process( "CTX", "fs", offsetof(Context,fs) );
    process( "CTX", "es", offsetof(Context,es) );
    process( "CTX", "ds", offsetof(Context,ds) );
    process( "CTX", "edi", offsetof(Context,edi) );
    process( "CTX", "esi", offsetof(Context,esi) );
    process( "CTX", "ebp", offsetof(Context,ebp) );
    process( "CTX", "esp", offsetof(Context,esp) );
    process( "CTX", "ebx", offsetof(Context,ebx) );
    process( "CTX", "edx", offsetof(Context,edx) );
    process( "CTX", "ecx", offsetof(Context,ecx) );
    process( "CTX", "eax", offsetof(Context,eax) );
    process( "CTX", "vector", offsetof(Context,vector) );
    process( "CTX", "code", offsetof(Context,code) );
    process( "CTX", "eip", offsetof(Context,eip) );
    process( "CTX", "cs", offsetof(Context,cs) );
    process( "CTX", "eflags", offsetof(Context,eflags) );
    fputc( '\n', genheader ? hfile : stdout );

    hsection( "PCB", "Pcb", sizeof(Pcb) );
    process( "PCB", "context", offsetof(Pcb,context) );
    process( "PCB", "stack", offsetof(Pcb,stack) );
    process( "PCB", "status", offsetof(Pcb,status) );
    process( "PCB", "wakeup", offsetof(Pcb,wakeup) );
    process( "PCB", "pid", offsetof(Pcb,pid) );
    process( "PCB", "ppid", offsetof(Pcb,ppid) );
    process( "PCB", "state", offsetof(Pcb,state) );
    process( "PCB", "priority", offsetof(Pcb,priority) );
    process( "PCB", "quantum",offsetof(Pcb,quantum) );
    process( "PCB", "filler", offsetof(Pcb,filler) );

    hsection( "ENT", "Entry", sizeof(Entry) );
    process( "ENT", "key", offsetof(Entry,key) );
    process( "ENT", "data", offsetof(Entry,data) );

    hsection( "VEC", "Vector", sizeof(Vector) );
    process( "VEC", "occupancy", offsetof(Vector,occupancy) );
    process( "VEC", "next", offsetof(Vector,next) );
    process( "VEC", "data", offsetof(Vector,data) );

    // finish up the offsets.h file if we need to
    if( genheader ) {
        fputs( h_suffix, hfile );
        fclose( hfile );
    }

    return( 0 );
}
