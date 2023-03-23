/**
** @file vector.h
**
** @author CSCI-452 class of 20225
**
** Vector type declarations
*/

#ifndef VECTOR_H_
#define VECTOR_H_

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
*/

#include "common.h"

/*
** Our Vector type is a simple vector of "things" held as an array of
** void* values.  A Vector node is the size of a VM slice (1024 bytes,
** currently) and contains an occupancy count, a pointer to another
** Vector node, and an array of Entry values.
*/

// Vector entry information
typedef
    struct entry_s {
        uint32_t key;   // key value describing this entry
        void *data;     // whatever data this Entry contains
    } Entry;

/*
** Calculate the number of Entry values we can hold in one 1024-byte
** slice.  In the 32-bit universe, uint32_t and pointers each occupy four
** bytes; this leaves 1016 bytes available to hold the array of Entry
** values.  Each Entry is eight bytes, so 1016/8 = 127 slots in the array.
*/

#define VEC_CAPACITY   ((SZ_SLICE - 8) / sizeof(Entry))

// The Vector type itself
typedef
    struct vector_s {
        uint32_t occupancy;         // contents of this node (4 bytes)
        struct vector_s *next;      // next node in sequence (4 bytes)
        Entry data[VEC_CAPACITY];   // vector contents (1024 - 8 bytes)
    } Vector;

/*
** Prototypes
*/

/**
** Name:  _vec_clear
**
** Clear (reset) a Vector.  Deallocates any extension nodes that
** are part of the vector.  Does nothing with the data and key
** values that are in the vector.
**
** @param vec    a pointer to the Vector to be updated
*/
void _vec_clear( Vector *vec );

/**
** Name:  _vec_add
**
** Add a value to the supplied Vector.  If the Vector is full, an
** extension node is allocated and the value is placed there.
**
** @param vec    a pointer to the Vector to be updated
** @param key    they key to add to the Vector
** @param data   the value to add to the Vector
*/
void _vec_add( Vector *vec, key_t key, void *data );

/**
** Name:  _vec_remove
**
** Remove the trailing element from a vector.  If this is the only
** value in an extension node, the extension node is deallocated
** as well.
**
** @param vec    a pointer to the Vector to be modified
**
** @return the value removed, or NULL
**
** Note: does not return the key associated with the data!
*/
void *_vec_remove( Vector *vec );

/**
** Name:  _vec_dump
**
** Dump the contents of a vector to the console.
**
** @param msg    an optional message to print
** @param vec    a pointer to the Vector to be examined
** @param all    print entire contents, or just the first few entries
*/
void _vec_dump( const char *msg, const Vector *vec, bool_t all );

#endif
/* SP_ASM_SRC */

#endif
