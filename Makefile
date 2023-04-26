#
# SCCS ID: @(#)Makefile	2.2	11/28/22
#
# Makefile to control the compiling, assembling and linking of standalone
# programs in the DSL.  Used for both individual interrupt handling
# assignments and the SP baseline OS (with appropriate tweaking).
#

##################
#  FILE SECTION  #
##################

#
# OS files
#

OS_C_SRC = clock.c kernel.c kmem.c processes.c queues.c sched.c \
	    sio.c stacks.c syscalls.c libc.c vector.c FileStructure/file_template.c
OS_C_OBJ = clock.o kernel.o kmem.o processes.o queues.o sched.o \
	    sio.o stacks.o syscalls.o vector.o file_template.o

OS_S_SRC =
OS_S_OBJ =

OS_HDRS  = clock.h common.h compat.h kdefs.h kernel.h kmem.h offsets.h \
	    processes.h queues.h sched.h sio.h stacks.h syscalls.h \
	    udefs.h ulib.h vector.h

OS_LIBS =

OS_SRCS = $(OS_C_SRC) $(OS_S_SRC)
OS_OBJS = $(OS_C_OBJ) $(OS_S_OBJ)

#
# "Userland" files
#

USR_C_SRC = users.c ulibc.c file_template.c
USR_C_OBJ = users.o ulibc.o file_template.h

USR_S_SRC = ulibs.S
USR_S_OBJ = ulibs.o

USR_HDRS  = udefs.h ulib.h users.h

USR_LIBS  =

USR_SRCS  = $(USR_C_SRC) $(USR_S_SRC)
USR_OBJS  = $(USR_C_OBJ) $(USR_S_OBJ)

#
# Framework files
#

FMK_S_SRC = startup.S isr_stubs.S libs.S
FMK_S_OBJ = startup.o isr_stubs.o libs.o

FMK_C_SRC = cio.c libc.c support.c
FMK_C_OBJ = cio.o libc.o support.o

FMK_HDRS = bootstrap.h cio.h lib.h support.h uart.h \
	   x86arch.h x86pic.h x86pit.h

BOOT_SRC = bootstrap.S
BOOT_OBJ = bootstrap.o

FMK_SRCS = $(FMK_S_SRC) $(FMK_C_SRC)
FMK_OBJS = $(FMK_S_OBJ) $(FMK_C_OBJ)

# Collections of files

OBJECTS = $(FMK_OBJS) $(OS_OBJS) $(USR_OBJS)

SOURCES = $(BOOT_SRC) $(FMK_SRCS) $(OS_SRCS) $(USR_SRCS)

#####################
#  OPTIONS SECTION  #
#####################

#
# Compilation/assembly definable options
#
# General options:
#	CLEAR_BSS		include code to clear all BSS space
#	GET_MMAP		get BIOS memory map via int 0x15 0xE820
#	SP_CONFIG		enable SP OS-specific startup variations
#
# Debugging options:
#	RPT_INT_UNEXP		report any 'unexpected' interrupts
#	RPT_INT_MYSTERY		report interrupt 0x27 specifically
#	TRACE_CX		include context restore trace code
#	TRACE=n			bitmask of internal tracing options
#       SANITY=n                enable "sanity check" level 'n'
#         0                     absolutely critical errors only
#         1                     important consistency checking
#         2                     less important consistency checking
#         > 2                   currently unused
#       USER_SHELL		spawn a user 'shell' process
#       CONSOLE_STATS		print statistics on console kbd input
#       STATUS=n                dump queue & process info every 'n' seconds
#
# Define SANITY as 0 for minimal runtime checking (critical errors only).
# If not defined, SANITY defaults to 9999.
#
# TRACE is a bitmask; see kdefs.h for definitions of the individual bits
# in the mask and the names of the available tracing macros.  Currently,
# these bits are defined:
#
#	PCB      0001	STACK    0002	QUEUE    0040	SCHED  0008
#	SYSCALLS 0010	SYSRETS  0020	EXIT     0040	DISP   0080
#	CONSOLE  0100	KMEM     0200	KM_FREE  0400	SPAWN  0800
#	SIO STAT 1000	SIO_ISR  2000	SIO_RD   4000	SIO_WR 8000
#
# You can add compilation options "on the fly" by using EXTRAS=thing
# on the command line.  For example, to compile with -H (to show the
# hierarchy of #includes):
#
# 	make EXTRAS=-H
#

GEN_OPTIONS = -DCLEAR_BSS -DGET_MMAP -DSP_CONFIG
DBG_OPTIONS = -DTRACE_CX -DSTATUS=3

USER_OPTIONS = $(GEN_OPTIONS) $(DBG_OPTIONS)

##############################################################
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!! #
##############################################################

#
# Compilation/assembly control
#

#
# We only want to include from the current directory
#
# INCLUDES = -I. -I/home/fac/wrc/include
INCLUDES = -I.

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

#
# Compiler/assembler/etc. settings for 32-bit binaries
#
CC = gcc
CFLAGS = -m32 -fno-pie -std=c99 -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS) $(EXTRAS)

AS = as
ASFLAGS = --32

LD = ld
LDFLAGS = -melf_i386 -no-pie

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#
# The .c.X rule produces a .X file which contains the original C source
# code from the file being compiled mixed in with the generated
# assembly language code.  Very helpful when you need to figure out
# exactly what C statement generated which assembly statements!
#

.SUFFIXES:	.S .b .X .i

.c.X:
	$(CC) $(CFLAGS) -g -c -Wa,-adhln $*.c > $*.X

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.c.i:
	$(CC) -E $(CFLAGS) -c $*.c > $*.i

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.img
#

usb.img: offsets.h bootstrap.b prog.b prog.nl BuildImage prog.dis 
	./BuildImage -d usb -o usb.img -b bootstrap.b prog.b 0x10000

floppy.img: offsets.h bootstrap.b prog.b prog.nl BuildImage prog.dis 
	./BuildImage -d floppy -o floppy.img -b bootstrap.b prog.b 0x10000

prog.out: $(OBJECTS)
	$(LD) $(LDFLAGS) -o prog.out $(OBJECTS)

prog.o:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o prog.o -Ttext 0x10000 $(OBJECTS) $(A_LIBS)

prog.b:	prog.o
	$(LD) $(LDFLAGS) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o

#
# Targets for copying bootable image onto boot devices
#

floppy:	floppy.img
	dd if=floppy.img of=/dev/fd0

usb:	usb.img
	/usr/local/dcs/bin/dcopy usb.img

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

Offsets:	Offsets.c processes.h stacks.h queues.h common.h vector.h
	$(CC) -mx32 -std=c99 $(INCLUDES) -o Offsets Offsets.c

offsets.h:	Offsets
	./Offsets -h

#
# Clean out this directory
#

clean:
	rm -f *.nl *.nll *.lst *.b *.i *.o *.X *.dis

realclean:	clean
	rm -f offsets.h *.img BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

prog.nl: prog.o
	nm -Bng prog.o | pr -w80 -3 > prog.nl

prog.nll: prog.o
	nm -Bn prog.o | pr -w80 -3 > prog.nll

#
# Generate a disassembly
#

prog.dis: prog.o
	objdump -d prog.o > prog.dis

#
# 'makedepend' is a program which creates dependency lists by looking
# at the #include lines in the source files.
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h
startup.o: bootstrap.h
isr_stubs.o: bootstrap.h
cio.o: cio.h lib.h support.h x86arch.h x86pic.h
libc.o: lib.h cio.h
support.o: support.h lib.h cio.h x86arch.h x86pic.h bootstrap.h
clock.o: x86arch.h x86pic.h x86pit.h common.h kdefs.h cio.h support.h lib.h
clock.o: kernel.h stacks.h kmem.h processes.h clock.h queues.h sched.h sio.h
clock.o: compat.h syscalls.h
kernel.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
kernel.o: kmem.h processes.h queues.h bootstrap.h clock.h sched.h sio.h
kernel.o: compat.h syscalls.h
kmem.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
kmem.o: kmem.h processes.h bootstrap.h
processes.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h
processes.o: stacks.h kmem.h processes.h sched.h queues.h vector.h
queues.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
queues.o: kmem.h processes.h queues.h vector.h
sched.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
sched.o: kmem.h processes.h sched.h queues.h
sio.o: compat.h ./uart.h x86arch.h x86pic.h sio.h common.h kdefs.h cio.h
sio.o: support.h lib.h kernel.h stacks.h kmem.h processes.h queues.h sched.h
stacks.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
stacks.o: kmem.h processes.h vector.h
syscalls.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h
syscalls.o: stacks.h kmem.h processes.h x86pic.h ./uart.h bootstrap.h
syscalls.o: syscalls.h queues.h sched.h clock.h sio.h compat.h
libc.o: lib.h cio.h
vector.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
vector.o: kmem.h processes.h vector.h
users.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
users.o: kmem.h processes.h users.h userland/main1.c ulib.h userland/main2.c
users.o: userland/main3.c userland/userH.c userland/userZ.c userland/userI.c
users.o: userland/userW.c userland/userJ.c userland/userY.c userland/main4.c
users.o: userland/userX.c userland/main5.c userland/userP.c userland/userQ.c
users.o: userland/userR.c userland/userS.c userland/main6.c userland/init.c
users.o: userland/idle.c FileStructure/file_template.c
ulibc.o: common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h stacks.h
ulibc.o: kmem.h processes.h
ulibs.o: syscalls.h common.h kdefs.h cio.h support.h lib.h kernel.h x86arch.h
ulibs.o: stacks.h kmem.h processes.h queues.h
