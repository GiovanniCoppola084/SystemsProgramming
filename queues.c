/**
** @file queues.c
**
** @author CSCI-452 class of 20225
**
** Queue module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "queues.h"
#include "kmem.h"
#include "vector.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** Queue organization
** ------------------
** Our queues are self-ordering, generic queues.  A queue can contain
** any type of data.  This is accomplished through the use of intermediate
** nodes called QNodes, which contain a void* data member, allowing them
** to point to any type of integral data (integers, pointers, etc.).
** The QNode list is doubly-linked for ease of traversal.
**
** Each Queue has associated with it a comparison function, which may be
** NULL.  Insertions into a Queue are handled according to this function.
** If the function pointer is NULL, the queue is FIFO, and the insertion
** is always done at the end of the queue.  Otherwise, the insertion is
** ordered according to the results from the comparison function.
**
** Neither of these types are visible to the rest of the system.  The
** Queue type is a pointer to the queue_s struct.
*/

//
// Internal QNode structure
//
typedef
    struct qnode_s {
        key_t key;              // used for ordering the QNode list
        void *data;             // pointer to whatever is being held here
        struct qnode_s *prev;   // backward link
        struct qnode_s *next;   // forward link
    } QNode;

#define SZ_QNODE    sizeof(struct qnode_s)

//
// A Queue
//
// (The Queue type is defined in queues.h)
//
struct queue_s {
    uint32_t count;         // current occupancy
    QNode *head;            // pointer to first QNode
    QNode *tail;            // pointer to last QNode
    int (*order)(key_t, key_t); // comparison function
};

#define SZ_QUEUE    sizeof(struct queue_s)

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** The list of free qnodes.
**
** Organized as a doubly-linked list using the 'next' pointer
** in the qnode structure.
*/
static QNode *_freeQnodes;

/*
** A vector containing free Queue structures.
*/
static Vector _freeQueues;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/**
** _qn_free() - return a QNode to the free list
**
** Deallocates the supplied qnode
**
** @param data   The QNode to be put on the free list (as a void*)
*/
static void _qn_free( void *data ) {

    // sanity check!
    assert1( data != NULL );

    // convert the void* into a QNode *
    QNode *node = (QNode *) data;

    // clean it out
    CLEAR_PTR( node );

    // stick this one at the front of the list
    node->next = _freeQnodes;
    _freeQnodes = node;
}

/**
** Name:  _qn_alloc
**
** Allocate a QNode
**
** @return A pointer to the allocated node, or NULL
*/
static QNode *_qn_alloc( void ) {

    // make sure there is one available to allocate
    if( _freeQnodes == NULL ) {
        // nope; allocate a slice
        assert( _grow_list(SZ_QNODE,_qn_free) );
    }

    // should have at least one on the free list
    QNode *tmp = _freeQnodes;
    assert2( tmp != NULL );

    // looks good - unlink it from the rest of the list
    _freeQnodes = tmp->next;
    tmp->next = NULL;

    // and return it
    return tmp;
}

/**
** Name:  _qi_search
**
** Search for a specific entry in the queue
**
** @param que   the Queue to be manipulated
** @param data  the data to be located
** @param how   a locater function
**
** @return a pointer to the located QNode, or NULL
*/
static QNode *_qi_search( Queue que, const void *data, LocateFcn how ) {

    // sanity check!
    assert( que != NULL );

    /*
    ** We will iterate through the Queue looking at
    ** each QNode using the how() function until either
    ** we run out of entries or how() returns true.
    */
    QNode *curr = que->head;

    while( curr != NULL && !how(curr->key,curr->data,data) ) {
        curr = curr->next;
    }

    // this is either a pointer to the located QNode, or NULL
    return curr;
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _q_init
**
** Initialize the Queue module
*/
void _q_init( void ) {

    // start by announcing ourselves
    __cio_puts( " Queue" );

    // initialize the set of available QNodes
    _freeQnodes = NULL;
    assert( _grow_list(SZ_QNODE,_qn_free) );

    // initialize the set of available Queues
    _vec_clear( &_freeQueues );
    assert( _grow_list(SZ_QUEUE,_q_free) );
}

/**
** Name:  _q_free
**
** Free a Queue structure
**
** @param data    The Queue to be freed (as a void*)
*/
void _q_free( void *data ) {
    // for convenience
    Queue que = (Queue) data;

#if TRACING_QUEUE
    __cio_printf( "** _q_free(%08x)\n", (uint32_t) data );
#endif

    // first, clear out the space
    CLEAR_PTR( que );

    // add it to the list
    _vec_add( &_freeQueues, 0, data );
#if TRACING_QUEUE
    _vec_dump( "** Vector:", &_freeQueues, false );
#endif
}

/**
** Name:  _q_alloc
**
** Allocate a Queue
**
** @param order    Ordering function for non-FIFO queues, or NULL
**
** @return A pointer to the new Queue
*/
Queue _q_alloc( OrderFcn order ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_alloc(%08x)", (uint32_t) order );
#endif

    // if there are no available queues, grow the free list
    if( _freeQueues.occupancy < 1 ) {
        assert( _grow_list(SZ_QUEUE,_q_free) );
    }

    // get the first thing out of the vector
    Queue tmp = (Queue) _vec_remove( &_freeQueues );
    assert1( tmp != NULL );

    // set up the ordering function
    tmp->order = order;
#if TRACING_QUEUE
    __cio_printf( " -> %08x\n", (uint32_t) tmp );
#endif

    return tmp;
}

/**
** Name:  _q_length
**
** Return the length of the specified queue
**
** @param que    The Queue to be examined
**
** @return The number of elements currently in the Queue
*/
uint32_t _q_length( Queue que ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_length(%08x)", (uint32_t) que );
#endif

    // sanity check!
    assert( que != NULL );

#if TRACING_QUEUE
    __cio_printf( " -> %d", que->count );
#endif

    // this one's pretty simple
    return( que->count );
}

/**
** Name:  _q_insert
**
** Perform an insertion (possibly sorted) into a Queue
**
** @param que   the Queue to be manipulated
** @param data  the data value to be inserted
** @param key   the key value associated with the data
**
** @return the number of elements in the Queue after the insertion
*/
uint32_t _q_insert( Queue que, void *data, key_t key ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_insert(%08x,%08x,%x)", (uint32_t) que,
            (uint32_t) data, key );
#endif

    // sanity check!
    assert1( que != NULL );

    // allocate a QNode
    QNode *qn = _qn_alloc();
    // if that failed, we can't continue
    assert1( qn != NULL );

    // fill in the data fields
    qn->data = data;
    qn->key = key;

    /*
    ** Do the insertion.  The simplest case is insertion
    ** into an empty queue.
    */

    if( que->count < 1 ) {
        que->count = 1;
        que->head = que->tail = qn;
        return que->count;
    }

    /*
    ** Inserting into a non-empty queue.
    **
    ** If this is an ordered queue, we must traverse it to find the
    ** insertion point; otherwise, we just append to whatever's there.
    */

    // pointer to what will follow the node we're inserting
    QNode *old = NULL;

    if( que->order != NULL ) {

        /* 
        ** This is an ordered queue.  We must traverse the list
        ** using the order() function to locate the insertion point.
        **
        ** order(old_key,qn->key):
        **      < 0 --> adding after old key (continue the search)
        **      = 0 --> adding after old key (continue the search)
        **      > 0 --> adding before old key (we're done!)
        */

        old = que->head;
        while( old && que->order(old->key,qn->key) <= 0 ) {
            old = old->next;
        }
    }

    /*
    ** If 'old' is NULL, we either didn't have an ordered queue, or
    ** we did and we are adding after the last element in the queue.
    */

    if( old == NULL ) {
        
        qn->prev = que->tail;   // predecessor of the new entry
        que->tail->next = qn;   // this is now the tail entry
        que->tail = qn;         // we're the successor of the old tail

    } else {

        qn->next = old;         // 'old' is our successor
        qn->prev = old->prev;   // 'old's predecessor is our predecessor
        old->prev = qn;         // we're 'old's predecessor now
    }

    que->count += 1;    // one more node in the queue
    return que->count;
}

/*
** Name:  _q_find
**
** Locate an entry in the queue
**
** @param que   the Queue to be examined
** @param data  the data to be located
** @param how   a locater function, or NULL
**
** @return a pointer to a Payload structure, or NULL
*/
Payload *_q_find( Queue que, const void *data, LocateFcn how ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_find(%08x,%08x,%08x)", (uint32_t) que,
            (uint32_t) data, (uint32_t) how );
#endif

    // sanity check
    assert( que != NULL );

    // if there is no locater function, return the first thing
    if( how == NULL ) {
#if TRACING_QUEUE
    __cio_printf( " -> %08x\n", (uint32_t) (que->head) );
#endif
        return (Payload *) (que->head);
    }

    // locate the desired entry
    QNode *ret = _qi_search( que, data, how );

#if TRACING_QUEUE
    __cio_printf( " -> %08x\n", (uint32_t) ret );
#endif

    // fun with types!
    return (Payload *) ret;
}

/**
** Name:  _q_remove
**
** Remove the first node from a Queue
**
** @param que   the Queue to be manipulated
** @param ret   a Payload structure into which the results are placed
**
** @return true if an element was removed; else, false
*/
bool_t _q_remove( Queue que, Payload *ret ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_find(%08x,%08x)", (uint32_t) que, (uint32_t) ret );
#endif

    // sanity check!
    assert( que != NULL );

    // empty queue means nothing to remove
    if( que->count < 1 ) {
#if TRACING_QUEUE
    __cio_puts( " -> EMPTY\n" );
#endif
        return false;
    }

    // grab the first QNode from the list
    QNode *tmp = que->head;

    // unlink it
    que->head = tmp->next;
    if( que->head == NULL ) {
        // nothing left!
        que->tail = NULL;
    } else {
        que->head->prev = NULL;
    }

    // make sure we remember that we removed something
    que->count -= 1;

    /*
    ** If the 'ret' parameter is non-NULL, the caller wants
    ** to know what was in the removed entry
    */
    if( ret != NULL ) {
        // playing fast and loose with the type system here...
        QNode *foo = (QNode *) ret;
        foo->key = tmp->key;
        foo->data = tmp->data;
#if TRACING_QUEUE
    __cio_printf( " -> (%x,%08x)\n", foo->key, foo->data );
#endif
    }

    // release the QNode!
    _qn_free( tmp );

    // another successful removal
    return true;
}

/**
** Name:  _q_removeBy
**
** Remove a node from a Queue using a "locater" function
**
** @param que   the Queue to be manipulated
** @param ret   a Payload structure into which the results are placed
** @param data  the data to be located
** @param how   a locater function
**
** @return true if an element was removed; else, false
*/
bool_t _q_removeBy( Queue que, Payload *ret, const void *data, LocateFcn how ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_remBy(%08x,%08x,%08x)", (uint32_t) que,
            (uint32_t) ret, (uint32_t) how );
#endif

    // sanity check!
    assert( que != NULL );

    // _qi_search() would handle this case, but this is faster
    if( que->count < 1 ) {
#if TRACING_QUEUE
    __cio_puts( " -> EMPTY\n" );
#endif
        return false;
    }

    /*
    ** We need to locate the QNode containing the data we want
    ** to remove from the Queue.  Call our search function to
    ** either find it, or let us know it didn't exist.
    */
    QNode *tmp = _qi_search( que, data, how );

    // did we find it?
    if( tmp != NULL ) {

        // yes! did the caller want the contents?
        if( ret != NULL ) {
            // more type follies
            QNode *foo = (QNode *) ret;
            foo->key = tmp->key;
            foo->data = tmp->data;
#if TRACING_QUEUE
    __cio_printf( " -> (%x,%08x)\n", foo->key, foo->data );
#endif
        }

        // get rid of the node
        _qn_free( tmp );
    }

    // indicate whether or not we removed something
    return tmp != NULL;
}

/**
** Name:  _q_forEach
**
** Iterate through a Queue, applying a processing function to each entry
**
** @param que   the Queue to be manipulated
** @param fcn   the processing function
*/
void _q_forEach( Queue que, ApplyFcn fcn ) {

#if TRACING_QUEUE
    __cio_printf( "** _q_apply(%08x,%08x)\n", (uint32_t) que, (uint32_t) fcn );
#endif

    // sanity check!
    assert( que != NULL );

    /*
    ** This is like _qi_search(), but processes the entire
    ** list of QNodes, calling the processing function on
    ** each one.
    */
    QNode *curr = que->head;

    while( curr != NULL ) {
        fcn( curr->key, curr->data );
        curr = curr->next;
    }

}

/*
** Debugging/tracing routines
*/

/**
** _q_dump(msg,que)
**
** dump the contents of the specified Queue to the console
**
** @param msg  Optional message to print
** @param que  Queue to dump
*/
void _q_dump( const char *msg, Queue que ) {

    // report on this queue
    __cio_printf( "%s: ", msg );
    if( que == NULL ) {
        __cio_puts( "NULL???\n" );
        return;
    }

    // first, the basic data
    __cio_printf( "head %08x tail %08x %d count",
                  (uint32_t) que->head, (uint32_t) que->tail, que->count );

    // next, how the queue is ordered
    if( que->order ) {
        __cio_printf( " order %08x\n", (uint32_t) que->order );
    } else {
        __cio_puts( " FIFO\n" );
    }

    // if there are members in the Queue, dump the first nodes
    if( que->count > 0 ) {
        __cio_puts( " data: " );
        QNode *tmp;
        int i = 0;
        for( tmp = que->head; i < 5 && tmp != NULL; ++i, tmp = tmp->next ) {
            __cio_printf( " [%x,%08x]", tmp->key, (uint32_t) tmp->data );
        }

        if( tmp != NULL ) {
            __cio_puts( " ..." );
        }

        __cio_putchar( '\n' );
    }
}
