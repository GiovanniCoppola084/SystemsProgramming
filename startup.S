/*
** SCCS ID:	@(#)startup.S	2.3	3/16/23
**
** File:	startup.S
**
** Author:	Jon Coles
**
** Contributor:	Warren R. Carithers, K. Reek
**
** Description:	SP startup code.
**
** This code prepares the various registers for execution of
** the program.  It sets up all the segment registers and the
** runtime stack.  By the time this code is running, we're in
** protected mode already.
*/
	.arch	i386

#include "bootstrap.h"

/*
** Configuration options - define in Makefile
**
**	CLEAR_BSS	include code to clear all BSS space
**	SP_CONFIG	enable SP OS-specific startup variations
*/

/*
** A symbol for locating the beginning of the code.
*/
	.globl begtext

	.text
begtext:

/*
** The entry point.
*/
	.globl	_start

_start:
	cli			/* seems to be reset on entry to p. mode */
	movb	$0x00, %al	/* re-enable NMIs (bootstrap */
	outb	$0x70		/*   turned them off) */

/*
** Set the data and stack segment registers (code segment register
** was set by the long jump that switched us into protected mode).
*/
	xorl	%eax, %eax	/* clear EAX */
	movw	$GDT_DATA, %ax	/* GDT entry #3 - data segment */
	movw	%ax, %ds	/* for all four data segment registers */
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	movw	$GDT_STACK, %ax	/* entry #4 is the stack segment */
	movw	%ax, %ss

	movl	$TARGET_STACK, %ebp	/* set up the system frame pointer */
	movl	%ebp, %esp	/* and stack pointer */

#ifdef CLEAR_BSS
/*
** Zero the BSS segment
**
** These symbols are defined automatically by the linker.
*/
	.globl	__bss_start, _end

	movl	$__bss_start, %edi
clearbss:
	movl	$0, (%edi)
	addl	$4, %edi
	cmpl	$_end, %edi
	jb	clearbss

#endif

#ifdef SP_CONFIG

/*
** Configuration for the baseline OS in the SP course.
**
** Call the OS initialization routine.
*/
	.globl	_kinit
	call	_kinit

/*
** Restore the first user process context.
**
** At this point, _kinit() must have created the first user
** process, and we're ready to shift into user mode.  The user
** stack for that process must have the initial context in it;
** we treat this as a "return from interrupt" event, and just
** transfer to the code that restores the user context.
*/
	jmp	__isr_restore   // defined in isr_stubs.S

#else

/*
** Configuration for the interrupt handling assignment in
** the OS course.
**
** Call the main() routine to initialize devices and do whatever
** this standalone program is supposed to do.
*/
	.globl	main
	call	main		// initialize the OS

/*
** Once we return from main(), we're done; print a message and lock up. 
*/
	cli			  // Disable interrupts
	pushl	$message	  // Print the message saying that
	pushl	$24		  //   the main function returned.
	movl	$80, %eax	  //   This goes to the lower right
	subl	$messagelen, %eax //   corner of the screen.
	pushl	%eax
	call	__cio_puts_at
die:	hlt			  // Stop.
	jmp	die

message:	.string "     ***** Returned from main! *****     "
messagelen = . - message - 1

#endif
