/**
** @file stacks.c
**
** @author CSCI-452 class of 20225
**
** Stack module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "kernel.h"
#include "stacks.h"
#include "vector.h"

// need the fallbackExit() entry point
extern int32_t fallbackExit( uint32_t, void * );

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// PCB management
static Vector _freeStacks;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/**
** _stk_init() - initialize the stack module
**
** Sets up the system stack (for use during interrupts)
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before interrupt handling has begun
**    Must be called before process creation has begun
*/
void _stk_init( void ) {

    __cio_puts( " Stacks" );

    // no preallocation here, so the initial free list is empty
    _vec_clear( &_freeStacks );

    // allocate the first stack for the OS
    _systemStack = _stk_alloc();
    assert( _systemStack != NULL );

    // set the initial ESP for the OS - it should point to the
    // next-to-last uint32 in the stack, so that when the code
    // at isr_save switches to the system stack and pushes the
    // error code and vector number, ESP is aligned at a multiple
    // of 16 address.
    _systemESP = ((uint32_t *) (_systemStack + 1)) - 2;
}

/**
** _stk_alloc() - allocate a stack
**
** @return a pointer to the allocated stack, or NULL
*/
Stack *_stk_alloc( void ) {

#if TRACING_STACK
    __cio_puts( "** _stk_alloc()\n" );
    _vec_dump( "** Free vec:", &_freeStacks, false );
#endif

    Stack *new;

    // see if there is an available stack
    if( _freeStacks.occupancy < 1 ) {

        // none available - create a new one
        new = (Stack *) _km_page_alloc( STACK_PAGES );
#if TRACING_STACK
    __cio_printf( "** new STK allocated %08x\n", (uint32_t) new );
#endif

    } else {

        // OK, we know that there is at least one free stack
        new = (Stack *) _vec_remove( &_freeStacks );
#if TRACING_STACK
    __cio_printf( "** STK reused %08x\n", (uint32_t) new );
    _vec_dump( "** Free vec:", &_freeStacks, false );
#endif

    }

    // clear out the fields in this one just to be safe
    CLEAR_PTR( new );

    // pass it back to the caller
    return( new );
}

/**
** _stk_free() - return a stack to the free list
**
** Deallocates the supplied stack
**
** @param data   The stack to be returned to the free list
*/
void _stk_free( void *data ) {

#if TRACING_STACK
    __cio_printf( "** STK freeing %08x\n", (uint32_t) data );
#endif

    // sanity check!
    if( data == NULL ) {
        return;
    }

    // add it to the free list
    _vec_add( &_freeStacks, 0, data );
}

/**
** _stk_setup - set up the stack for a new process
**
** @param stk   The stack to be set up
** @param len   Length of the argument buffer
** @param arg   Argument buffer
**
** @return A pointer to the Context on the stack, or NULL
*/
Context *_stk_setup( Stack *stk, uint32_t len, void *buffer ) {

#if TRACING_STACK
    __cio_printf( "** STK setup 0x%08x, args %u 0x%08x\n",
            (uint32_t) stk, len, (uint32_t) buffer );
#endif
    // verify our parameters
	//
    // first, the buffer length
    if( len > MAX_ARG_LEN ) {
#if TRACING_STACK
    __cio_puts( "** BAD LEN\n" );
#endif
        return NULL;
    }

    // next, the stack
    if( stk == NULL ) {
        // should we blow up here?
#if TRACING_STACK
    __cio_puts( "** BAD stk\n" );
#endif
        return NULL;
    }

    // finally, the buffer pointer and length must agree
    if( (len > 0 && buffer == NULL) || (len == 0 && buffer != NULL) ) {
        // should we blow up here?
#if TRACING_STACK
    __cio_puts( "** len/buffer disagreement\n" );
#endif
        return NULL;
    }

    /*
    ** We need to create the initial stack for the process.
    **
    ** We reserve one longword at the bottom of the stack and
    ** put the integer 0 there.
    **
    ** Above that, we simulate a call from fallbackExit() with an
    ** argument vector by pushing the arguments and then the argument
    ** length.  We follow this up by pushing the address of the entry point
    ** of fallbackExit() as a "return address".  Above that, we place a
    ** Context area that holds the standard initial register contents.
    **
    ** The low end of the stack will look like this:
    **
    **      esp ->  context      <- context save area
    **              ...          <- context save area
    **              context      <- context save area
    **              fallbackExit <- return address for faked call to main()
    **              arglen       <- argument buffer length for main()
    **         /--  argptr       <- argument buffer for main()
    **         |    ....         <- filler bytes
    **         \->  b.b.b.b.     <- arg buffer bytes
    **               ...         <- argv character strings
    **              ....         <- filler bytes
    **                 0         <- last word in stack
    **
    ** Each 'b' is one byte of the argument buffer.
    **
    ** The areas marked as "filler" are there because of the need to
    ** preserve alignment.
    **
    ** Stack alignment is controlled by the SysV ABI i386 supplement,
    ** version 1.2 (June 23, 2016), which states in section 2.2.2:
    **
    **   "The end of the input argument area shall be aligned on a 16
    **   (32 or 64, if __m256 or __m512 is passed on stack) byte boundary.
    **   In other words, the value (%esp + 4) is always a multiple of 16
    **   (32 or 64) when control is transferred to the function entry
    **   point. The stack pointer, %esp, always points to the end of the
    **   latest allocated stack frame."
    **
    ** Isn't technical documentation fun?  Ultimately, this means that
    ** the first parameter to main() should be on the stack at an address
    ** that is a multiple of 16.
    **
    ** The space needed for arglen and the arg pointer is 8 bytes; we need
    ** to put the arg buffer itself at another multiple-of-16 address
    ** below that (in case it's a struct or something else that requires
    ** such an alignment), so we have eight filler bytes after the two
    ** arguments.  We also may need filler bytes between the argument
    ** buffer and the "zero word".
    */

    // create a pointer to the last longword in this stack
	uint32_t *ptr = ((uint32_t *) (stk + 1)) - 1;
#if TRACING_STACK
    __cio_printf( "** stk %08x +1 %08x, ptr %08x\n",
            (uint32_t) stk, (uint32_t) (stk + 1), (uint32_t) ptr );
#endif
	
    // copy a 0 there and back up one longword
	*ptr-- = 0;

    /*
    ** Figure out where the argument buffer should be copied.  The
    ** buffer must begin at a multiple-of-16 address.  We figure out
    ** where it is by backing our pointer up by the buffer length
    ** ('len'), and then rounding that address back to the nearest
    ** preceding multiple of 16.
    **
    ** The simplest way to do this is to back the pointer up by
    ** 'len' bytes, and then just clearing the low-order four bits
    ** of the address.
    */
	
	ptr = (uint32_t *) ( ( ((uint32_t) ptr) - len ) & MOD16_MASK );
#if TRACING_STACK
    __cio_printf( "** reserve %u, ptr now %08x\n",
            len, (uint32_t) ptr );
#endif
	
    // copy in the buffer, if there is one
    if( len > 0 ) {
        // do the copy
	    __memcpy( ptr, buffer, len );
    }

    /*
    ** Now, put the parameters in place.  These will be 16 bytes
    ** before the buffer data in the stack; we'll let the compiler
    ** do the heavy lifting of address calculation for us here. :-)
    **
    ** If there is no argument (len == 0 and buffer == NULL), we
    ** call main(0,0); else, we call main(len,bufptr).
    **
    ** Note: we are going to waste some space here if there is no
    ** buffer to be passed in because the length and "buffer pointer"
    ** would have fit into the first eight bytes of the space we just
    ** reserved for the buffer, but we go ahead and back up 16 more
    ** bytes anyway.  Such is life.
    */

#if TRACING_STACK
    __cio_printf( "** ret @ %08x, arglen @ %08x, args @ %08x\n",
            (uint32_t) (ptr-5), (uint32_t) (ptr-4), (uint32_t) (ptr-3) );
#endif
    // main( len, buffer )
	ptr[-4] = len;
	ptr[-3] = len > 0 ? (uint32_t) ptr : 0;
	
    // back up to where the return address will be
	ptr -= 5;
    *ptr = (uint32_t) fallbackExit;
	
    // finally, back up by the length of a Context to reserve that space
	Context *ctx = ((Context *) ptr) - 1;
#if TRACING_STACK
    __cio_printf( "** context @ %08x\n", (uint32_t) ctx );
#endif
	
    // all done!
	return ctx;
}

/*
** Debugging/tracing routines
*/

/**
** _stk_dump(msg,stk,lim)
**
** Dumps the contents of this stack to the console.  Assumes the stack
** is a multiple of four words in length.
**
** @param msg   An optional message to print before the dump
** @param stk   The stack to dump out
** @param lim   Limit on the number of words to dump (0 for all)
*/

// buffer sizes (rounded up a bit)
#define HBUFSZ      48
#define CBUFSZ      24

void _stk_dump( const char *msg, Stack *stk, uint32_t limit ) {
    int words = STACK_WORDS;
    int eliding = 0;
    char oldbuf[HBUFSZ], buf[HBUFSZ], cbuf[CBUFSZ];
    uint32_t addr = (uint32_t ) stk;
    uint32_t *sp = (uint32_t *) stk;
    char hexdigits[] = "0123456789ABCDEF";

    // if a limit was specified, dump only that many words

    if( limit > 0 ) {
        words = limit;
        if( (words & 0x3) != 0 ) {
            // round up to a multiple of four
            words = (words & 0xfffffffc) + 4;
        }
        // skip to the new starting point
        sp += (STACK_WORDS - words);
        addr = (uint32_t) sp;
    }

    __cio_puts( "*** stack" );
    if( msg != NULL ) {
        __cio_printf( " (%s):\n", msg );
    } else {
        __cio_puts( ":\n" );
    }

    /**
    ** Output lines begin with the 8-digit address, followed by a hex
    ** interpretation then a character interpretation of four words:
    **
    ** aaaaaaaa*..xxxxxxxx..xxxxxxxx..xxxxxxxx..xxxxxxxx..cccc.cccc.cccc.cccc
    **
    ** Output lines that are identical except for the address are elided;
    ** the next non-identical output line will have a '*' after the 8-digit
    ** address field (where the '*' is in the example above).
    */

    oldbuf[0] = '\0';

    while( words > 0 ) {
        register char *bp = buf;   // start of hex field
        register char *cp = cbuf;  // start of character field
        uint32_t start_addr = addr;

        // iterate through the words for this line

        for( int i = 0; i < 4; ++i ) {
            register uint32_t curr = *sp++;
            register uint32_t data = curr;

            // convert the hex representation

            // two spaces before each entry
            *bp++ = ' ';
            *bp++ = ' ';

            for( int j = 0; j < 8; ++j ) {
                uint32_t value = (data >> 28) & 0xf;
                *bp++ = hexdigits[value];
                data <<= 4;
            }

            // now, convert the character version
            data = curr;

            // one space before each entry
            *cp++ = ' ';

            for( int j = 0; j < 4; ++j ) {
                uint32_t value = (data >> 24) & 0xff;
                *cp++ = (value >= ' ' && value < 0x7f) ? (char) value : '.';
                data <<= 8;
            }
        }
        *bp = '\0';
        *cp = '\0';
        words -= 4;
        addr += 16;

        // if this line looks like the last one, skip it

        if( __strcmp(oldbuf,buf) == 0 ) {
            ++eliding;
            continue;
        }

        // it's different, so print it

        // start with the address
        __cio_printf( "%08x%c", start_addr, eliding ? '*' : ' ' );
        eliding = 0;

        // print the words
        __cio_printf( "%s %s\n", buf, cbuf );

        // remember this line
        __memcpy( (uint8_t *) oldbuf, (uint8_t *) buf, HBUFSZ );
    }
}
