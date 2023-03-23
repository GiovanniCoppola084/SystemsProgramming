/*
** SCCS ID: @(#)libc.c	2.4        3/10/23
**
** @file libc.c
**
** @author  Warren R. Carithers
**
** Library of support functions (C language)
**
** This module implements a simple collection of support functions
** similar to the standard C library.
**
*/

#include "lib.h"
#include "cio.h"

/**
** Name:    __put_char_or_code( ch )
**
** Description: Prints a character on the console, unless it
** is a non-printing character, in which case its hex code
** is printed
**
** @param ch    The character to be printed
*/
void __put_char_or_code( int ch ) {

    if( ch >= ' ' && ch < 0x7f ) {
        __cio_putchar( ch );
    } else {
        __cio_printf( "\\x%02x", ch );
    }
}

/**
** Name:    __bound
**
** Description: This function confines an argument within specified bounds.
**
** @param min    Lower bound
** @param value  Value to be constrained
** @param max    Upper bound
**
** @return The constrained value
*/
unsigned int __bound( unsigned int min, unsigned int value,
                        unsigned int max ){
    if( value < min ){
        value = min;
    }
    if( value > max ){
        value = max;
    }
    return value;
}

/**
** Name:        __memset
**
** Description: initialize all bytes of a block of memory to a specific value
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
** @param value  Initialization value
*/
void __memset( void *buf, register unsigned int len,
               register unsigned int value ) {
    register unsigned char *bp = buf;

    /*
    ** We could speed this up by unrolling it and copying
    ** words at a time (instead of bytes).
    */

    while( len-- ) {
        *bp++ = value;
    }
}

/**
** Name:        __memclr
**
** Description: Initialize all bytes of a block of memory to zero
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
*/
void __memclr( void *buf, register unsigned int len ) {
    register unsigned char *dest = buf;

    /*
    ** We could speed this up by unrolling it and clearing
    ** words at a time (instead of bytes).
    */

    while( len-- ) {
            *dest++ = 0;
    }

}

/**
** Name:        __memcpy
**
** Description: Copy a block from one place to another
**
** May not correctly deal with overlapping buffers
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void __memcpy( void *dst, register const void *src,
               register unsigned int len ) {
    register unsigned char *dest = dst;
    register const unsigned char *source = src;

    /*
    ** We could speed this up by unrolling it and copying
    ** words at a time (instead of bytes).
    */

    while( len-- ) {
        *dest++ = *source++;
    }

}

/**
** Name:        __strlen
**
** Description: Calculate the length of a C-style string.
**
** @param str  The string to examine
**
** @return The length of the string
*/

unsigned int __strlen( register const char *str ){
    register unsigned int len = 0;

    while( *str++ ){
        ++len;
    }
    return len;
}

/**
** Name:        __strcmp
**
** Description: Compare two strings
**
** @param s1   The first string to examine
** @param s2   The second string to examine
**
** @return < 0 if s1 < s2; 0 if equal; > 0 if s1 > s2
*/
int __strcmp( register const char *s1, register const char *s2 ) {

    while( *s1 && (*s1 == *s2) )
        ++s1, ++s2;

    return( *s1 - *s2 );
}

/**
** Name:        __strcpy
**
** Description: Copy a string into a destination buffer
**
** May not correctly deal with overlapping buffers
**
** @param dst   The destination buffer
** @param src   The source buffer
**
** @return The destination buffer
*/
char *__strcpy( register char *dst, register const char *src ) {
    char *tmp = dst;

    while( (*dst++ = *src++) )
        ;

    return( tmp );
}

/**
** Name:    _strcat
**
** Description: Append one string to another
**
** @param dst   The destination buffer
** @param src   The source buffer
**
** @return The destination buffer
**
** NOTE:  May not correctly deal with overlapping buffers.  Assumes
** dst is large enough to hold the resulting string.
*/
char *__strcat( register char *dst, register const char *src ) {
    register char *tmp = dst;

    while( *dst )  // find the NUL
        ++dst;

    while( (*dst++ = *src++) )  // append the src string
        ;

    return( tmp );
}

/**
** Name:    __pad
**
** Description: Generate a padding string
**
** @param dst     Pointer to where the padding should begin
** @param extra   How many padding bytes to add
** @param padchar What character to pad with
**
** @return Pointer to the first byte after the padding
*/
char *__pad( char *dst, int extra, int padchar ) {
    while( extra > 0 ){
        *dst++ = (char) padchar;
        extra -= 1;
    }
    return dst;
}

/**
** Name:    __padstr
**
** Description: Add padding characters to a string
**
** @param dst        The destination buffer
** @param str        The string to be padded
** @param len        The string length, or -1
** @param width      The desired final length of the string
** @param leftadjust Should the string be left-justified?
** @param padchar    What character to pad with
**
** @return Pointer to the first byte after the padded string
*/
char *__padstr( char *dst, char *str, int len, int width,
                   int leftadjust, int padchar ) {
    int    extra;

    // determine the length of the string if we need to
    if( len < 0 ){
        len = __strlen( str );
    }

    // how much filler must we add?
    extra = width - len;

    // add filler on the left if we're not left-justifying
    if( extra > 0 && !leftadjust ){
        dst = __pad( dst, extra, padchar );
    }

    // copy the string itself
    for( int i = 0; i < len; ++i ) {
        *dst++ = str[i];
    }

    // add filler on the right if we are left-justifying
    if( extra > 0 && leftadjust ){
        dst = __pad( dst, extra, padchar );
    }

    return dst;
}

/**
** Name:        __sprint
**
** Description: Formatted output into a string buffer
**
** @param dst   The destination buffer
** @param fmt   The format string
**
** The format string parameter is followed by zero or more additional
** parameters which are interpreted according to the format string.
**
** NOTE:  assumes the buffer is large enough to hold the result string
**
** NOTE:  relies heavily on the x86 convention that parameters
** are pushed onto the stack in reverse order as 32-bit values.
*/
void __sprint( char *dst, char *fmt, ... ) {
    int *ap;
    char buf[ 12 ];
    char ch;
    char *str;
    int leftadjust;
    int width;
    int len;
    int padchar;

    /*
    ** Get characters from the format string and process them
    **
    ** We use the "old-school" method of handling variable numbers
    ** of parameters.  We assume that parameters are passed on the
    ** runtime stack in consecutive longwords; thus, if the first
    ** parameter is at location 'x', the second is at 'x+4', the
    ** third at 'x+8', etc.  We use a pointer to a 32-bit thing
    ** to point to the next "thing", and interpret it according
    ** to the format string.
    */

    // get the pointer to the first "value" parameter
    ap = (int *)(&fmt) + 1;

    // iterate through the format string
    while( (ch = *fmt++) != '\0' ){
        /*
        ** Is it the start of a format code?
        */
        if( ch == '%' ){
            /*
           ** Yes, get the padding and width options (if there).
            ** Alignment must come at the beginning, then fill,
            ** then width.
            */
            leftadjust = 0;
            padchar = ' ';
            width = 0;
            ch = *fmt++;
            if( ch == '-' ){
                leftadjust = 1;
                ch = *fmt++;
            }
            if( ch == '0' ){
                padchar = '0';
                ch = *fmt++;
            }
            while( ch >= '0' && ch <= '9' ){
                width *= 10;
                width += ch - '0';
                ch = *fmt++;
            }

            /*
            ** What data type do we have?
            */
            switch( ch ) {

            case 'c':  // characters are passed as 32-bit values
                ch = *ap++;
                buf[ 0 ] = ch;
                buf[ 1 ] = '\0';
                dst = __padstr( dst, buf, 1, width, leftadjust, padchar );
                break;

            case 'd':
                len = __cvtdec( buf, *ap++ );
                dst = __padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            case 's':
                str = (char *) (*ap++);
                dst = __padstr( dst, str, -1, width, leftadjust, padchar );
                break;

            case 'x':
                len = __cvthex( buf, *ap++ );
                dst = __padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            case 'o':
                len = __cvtoct( buf, *ap++ );
                dst = __padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            case 'u':
                len = __cvtuns( buf, *ap++ );
                dst = __padstr( dst, buf, len, width, leftadjust, padchar );
                break;

            }
        } else {
            // no, it's just an ordinary character
            *dst++ = ch;
        }
    }

    // NUL-terminate the result
    *dst = '\0';
}

/**
** Conversion functions
*/

/**
** Name:    __cvtdec0
**
** Description: Convert an integer value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
char *__cvtdec0( char *buf, int value ){
    int quotient;

    quotient = value / 10;
    if( quotient < 0 ){
        quotient = 214748364;
        value = 8;
    }
    if( quotient != 0 ){
        buf = __cvtdec0( buf, quotient );
    }
    *buf++ = value % 10 + '0';
    return buf;
}

/**
** Name:    __cvtuns0
**
** Description: Convert an unsigned int value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
char *__cvtuns0( char *buf, unsigned int value ){
    unsigned int quotient;

    quotient = value / 10;
    if( quotient != 0 ){
        buf = __cvtdec0( buf, quotient );
    }
    *buf++ = value % 10 + '0';
    return buf;
}

/**
** Name:    __cvtdec
**
** Description: Convert an integer value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
int __cvtdec( char *buf, int value ){
    char    *bp = buf;

    if( value < 0 ){
        *bp++ = '-';
        value = -value;
    }
    bp = __cvtdec0( bp, value );
    *bp = '\0';

    return bp - buf;
}

/**
** Name:    __cvtuns
**
** Description: Convert an unsigned int value into a decimal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The result buffer
*/
int __cvtuns( char *buf, unsigned int value ){
    char    *bp = buf;

    bp = __cvtuns0( bp, value );
    *bp = '\0';

    return bp - buf;
}

/**
** Array of hex digit characters (used by __cvthex() and others)
*/
char __hexdigits[] = "0123456789ABCDEF";

/**
** Name:    __cvthex
**
** Description: Convert an integer value into a hex character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The buffer length
*/
int __cvthex( char *buf, int value ){
    int i;
    int chars_stored = 0;
    char    *bp = buf;

    for( i = 0; i < 8; i += 1 ){
        int val;

        val = ( value & 0xf0000000 );
        if( i == 7 || val != 0 || chars_stored ){
            chars_stored = 1;
            val >>= 28;
            val &= 0xf;
            *bp++ = __hexdigits[ val ];
        }
        value <<= 4;
    }
    *bp = '\0';

    return bp - buf;
}

/**
** Name:    __cvtoct
**
** Description: Convert an integer value into an octal character string.
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return The buffer length
*/
int __cvtoct( char *buf, int value ){
    int i;
    int chars_stored = 0;
    char    *bp = buf;
    int val;

    val = ( value & 0xc0000000 );
    val >>= 30;
    for( i = 0; i < 11; i += 1 ){

        if( i == 10 || val != 0 || chars_stored ){
            chars_stored = 1;
            val &= 0x7;
            *bp++ = __hexdigits[ val ];
        }
        value <<= 3;
        val = ( value & 0xe0000000 );
        val >>= 29;
    }
    *bp = '\0';

    return bp - buf;
}
