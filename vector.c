/**
** @file vector.c
**
** @author CSCI-452 class of 20225
**
** Vector implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "vector.h"
#include "kmem.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  _deallocExtension
**
** Recursively deallocates extension nodes.
**
** @param node   Pointer to the first extension node.
*/
static void _deallocExtension( Vector *node ) {

    // if NULL, we're done
    if( node == NULL ) {
        return;
    }

    // is there another one beyond this?
    if( node->next != NULL ) {
        // yes - recursively deallocate the chain of nodes
        _deallocExtension( node->next );
    }

    // can now free the Vector
    _km_slice_free( node );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _vec_clear
**
** Clear (reset) a Vector
**
** @param vec    a pointer to the Vector to be updated
*/
void _vec_clear( Vector *vec ) {

    // do nothing if the parameter is bad
    if( vec == NULL ) {
        return;
    }

    vec->occupancy = 0;

    // if 'next' isn't NULL, we have one (or more) extension nodes
    if( vec->next != NULL ) {
        _deallocExtension( vec->next );
        vec->next = NULL;
    }

    for( int i = 0; i < VEC_CAPACITY; ++i ) {
        vec->data[i].key  = 0;
        vec->data[i].data = NULL;
    }
}

/**
** Name:  _vec_add
**
** Add a value to the supplied Vector.  If the Vector is full, an
** extension node is allocated and the value is placed there.
**
** @param vec    a pointer to the Vector to be updated
** @param key    the key to add to the Vector
** @param data   the value to add to the Vector
*/
void _vec_add( Vector *vec, key_t key, void *data ) {
    
    // must have a Vector!
    assert1( vec != NULL );

    // find the insertion point
    while( vec->occupancy >= VEC_CAPACITY ) {

        // this node is full, so we need to move to the next one
        if( vec->next == NULL ) {

            // there isn't one - create it
            vec->next = (Vector *) _km_slice_alloc();
            assert1( vec->next != NULL );

            // make sure it appears to be empty!
            vec->next->occupancy = 0;
            vec->next->next = NULL;

        }

        // move to the successor
        vec = vec->next;
    }

    // copy the data into the vector
    vec->data[ vec->occupancy ].key  = key;
    vec->data[ vec->occupancy ].data = data;
    vec->occupancy += 1;
}

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
void *_vec_remove( Vector *vec ) {

    // must have a Vector!
    assert1( vec != NULL );

    // quick check: is this Vector empty?
    if( vec->occupancy < 1 ) {
        // yes, so nothing to remove
        return NULL;
    }

    /*
    ** OK, we have a non-empty Vector.  We need to find the last
    ** Vector node in the chain of nodes so that we can remove
    ** the last value from it.  This means we may need to follow
    ** the chain.  If we were in an ordinary application, this would
    ** be an ideal place to do a recursive removal, but we're not,
    ** so we won't. :-)
    **
    ** Instead, we loop here until we find the last node in the
    ** list of nodes.  There are two situations to watch for:
    **
    **   - the node is not full
    **   - the node is full and there is no successor
    */

    Vector *prev = NULL;
    while( vec->occupancy >= VEC_CAPACITY ) {

        // This node is full - is there a successor?
        if( vec->next != NULL ) {

            // Yes, so move to it and loop
            prev = vec;
            vec = vec->next;

        } else {

            // no successor, so we'll remove from the current node.
            break;

        }
    }

    /*
    ** At this point, 'vec' points to the node we're removing from,
    ** and 'prev' either points to the preceding node or is NULL.
    **
    ** In theory, the last node should not be empty, but we'll check
    ** for that anyway.
    */

    if( vec->occupancy < 1 ) {

        /*
        ** This is a strange situation.  We have traversed the
        ** chain of nodes, and have come to a node that has
        ** nothing in it.  Because we checked for an empty Vector
        ** at the beginning of the function, this means we have
        ** followed a chain of Vector nodes and come to one that
        ** has nothing in it, which should not be possible.
        **
        ** We have several options.  We could just blow up ("this
        ** can't happen! help!").  Instead, we'll see if this node
        ** just didn't get removed when it should have been removed,
        ** and then go back to its predecessor.
        **
        ** If there is no predecessor, though, we're toast.
        */

        assert1( prev != NULL );

        // get rid of the dangling empty node
        _km_slice_free( vec );

        // go back one step
        vec = prev;
        prev = NULL;

        // make sure this node is marked as being the end point
        vec->next = NULL;
    }

    // remove the last entry
    vec->occupancy -= 1;
    void *data = vec->data[vec->occupancy].data;

    // if this was the only entry, back up (if we can) and
    // get rid of this now-empty node from the list

    if( vec->occupancy < 1 && prev != NULL ) {
        // remove the now-empty node
        _km_slice_free( vec );
        prev->next = NULL;
    }

    return data;
}

/**
** Name:  _vec_dump
**
** Dump the contents of a vector to the console.
**
** @param msg    an optional message to print
** @param vec    a pointer to the Vector to be examined
** @param all    print entire contents, or just the first few entries
*/
void _vec_dump( const char *msg, const Vector *vec, bool_t all ) {

    // print the message, if there is one
    if( msg != NULL ) {
        __cio_puts( msg );
    }

    // no vector means nothing else to do!
    if( vec == NULL ) {
        __cio_puts( " is NULL\n" );
        return;
    }

    // statistics
    __cio_printf( " Occupancy %d/%d", vec->occupancy, VEC_CAPACITY );

    if( vec->next != NULL ) {
        __cio_printf( " extension at 0x%08x", (uint32_t) (vec->next) );
    }
    __cio_puts( "\n" );

    // report on the contents
    int n = 0;

    while( n < vec->occupancy ) {
        if( !all && n > 4 ) {
            __cio_puts( "..." );
            break;
        }
        __cio_printf( " [%x,%08x]", vec->data[n].key, vec->data[n].data );
        ++n;
    }
    __cio_puts( "\n" );
}
