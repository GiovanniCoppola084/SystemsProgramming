/*
** SCCS ID:	@(#)libs.S	2.2	11/29/22
**
** @file libs.S
**
** @author Jon Coles
** @author Warren R. Carithers
** @author K. Reek
**
** Support library functions (assembly language)
**
** These functions are support routines used in various places
** throughout the framework.  They are written in assembly language
** for efficiency and/or because they require access to machine-level
** features that are hard to access from C.
*/

/*
** Parameter offsets
*/
ARG1	= 8			// Offset to 1st argument
ARG2	= 12			// Offset to 2nd argument

/**
** Name:	__inb, __inw, __inl
**
** Description: read a single byte, word, or longword from the specified
**		input port
**
** usage:  data = __in*( unsigned short port );
**
** @param port   The port from which to read
**
** @return The data from that port
*/
	.globl	__inb, __inw, __inl

__inb:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inb	(%dx)		// Get a byte from the port into %al (low
	leave			//   byte of %eax)
	ret
__inw:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inw	(%dx)		// Get a word from the port into %ax (low
	leave			//   word of %eax)
	ret
__inl:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inl	(%dx)		// Get a longword from the port into %eax
	leave
	ret
	
/**
** Name:	__outb, __outw, __outl
**
** Description: write a single byte, word, or longword to the specified
**		output port
**
** usage:  __out*( unsigned short port, unsigned data );
**
** @param port   The port to be written to
** @param data   The data to write to that port
*/
	.globl	__outb, __outw, __outl
__outb:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outb	(%dx)		// Output that byte to the port
	leave			//   (only %al is sent)
	ret
__outw:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outw	(%dx)		// Output that word to the port.
	leave			//   (only %ax is sent)
	ret
__outl:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outl	(%dx)		// Output that longword to the port.
	leave
	ret

/**
** Name:    __get_flags
**
** Description: Get the current processor flags
**
** @return The EFLAGS register after entry to this function
*/
	.globl	__get_flags

__get_flags:
	pushfl			// Push flags on the stack,
	popl	%eax		//   and pop them into eax.
	ret

/**
** Name:    __pause
**
** Description: Pause until something happens
*/
	.globl	__pause

__pause:
	enter	$0,$0
	sti
	hlt
	leave
	ret

/**
** __get_ra:
**
** Description: Get the return address for the calling function
**              (i.e., where whoever called us will go back to)
**
** @return The address the calling routine will return to as a uint32_t
*/
	.global	__get_ra

__get_ra:
	// DO NOT create a stack frame - use the caller's
	//
	// Caller's return address is between the saved EBP
	// and its first parameter
	movl	4(%ebp), %eax
	ret
