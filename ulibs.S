/**
** @file ulibs.S
**
** @author Numerous CSCI-452 classes
**
** assembly-language user-level library functions
*/

#define	SP_ASM_SRC

// get the system call codes

#include "syscalls.h"

/**
** System call stubs
**
** All have the same structure:
**
**      move a code into EAX
**      generate the interrupt
**      return to the caller
**
** As these are simple "leaf" routines, we don't use
** the standard enter/leave method to set up a stack
** frame - that takes time, and we don't really need it.
*/

#define	SYSCALL(name) \
	.globl	name			; \
name:					; \
	movl	$SYS_##name, %eax	; \
	int	$INT_VEC_SYSCALL	; \
	ret

/*
** "real" system calls
*/

SYSCALL(exit)
SYSCALL(spawn)
SYSCALL(read)
SYSCALL(write)
SYSCALL(sleep)
SYSCALL(kill)
SYSCALL(waitpid)

/*
** This one is kind of a kludge - the user shell process
** calls it to register itself, so that the kernel shell
** that responds to console keyboard input knows which
** process to dispatch when the "run user shell" command
** is given.
*/
SYSCALL(ushell)

/*
** This is a bogus system call; it's here so that we can test
** our handling of out-of-range syscall codes in the syscall ISR.
*/
SYSCALL(bogus)

/*
** Other library functions
*/

/**
** fallbackExit() - dummy "startup" function
**
** calls exit(FAKE_EXIT) - serves as the "return to" code for
** main() functions, in case they don't call exit() themselves
*/

        .globl  fallbackExit
fallbackExit:
        // alternate: use 'pushl %eax' to return
        // whatever value is in %eax
        pushl   $FAKE_EXIT      // termination status
        call    exit            // terminate this process
