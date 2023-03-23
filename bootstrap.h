/*
** SCCS ID:	@(#)bootstrap.h	2.2	11/28/22
**
** File:	bootstrap.h
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers, Garrett C. Smith
**
** Description:	Addresses where various stuff goes in memory.
*/

#ifndef	BOOTSTRAP_H_
#define	BOOTSTRAP_H_

/*
** The boot device
*/
#define BDEV_FLOPPY     0x00
#define BDEV_USB        0x80

#define BDEV            BDEV_USB

/*
** The target program itself
*/
#define TARGET_SEGMENT	0x00001000	/* 1000:0000 */
#define TARGET_ADDRESS	0x00010000	/* and upward */
#define	TARGET_STACK	0x00010000	/* and downward */

/*
** The Global Descriptor Table (0000:0500 - 0000:2500)
*/
#define	GDT_SEGMENT	0x00000050
#define	GDT_ADDRESS	0x00000500

	/* segment register values */
#define	GDT_LINEAR	0x0008		/* All of memory, R/W */
#define	GDT_CODE	0x0010		/* All of memory, R/E */
#define	GDT_DATA	0x0018		/* All of memory, R/W */
#define	GDT_STACK	0x0020		/* All of memory, R/W */

/*
** The Interrupt Descriptor Table (0000:2500 - 0000:2D00)
*/
#define	IDT_SEGMENT	0x00000250
#define IDT_ADDRESS	0x00002500

/*
** Physical Memory Map Table (0000:2D00 - 0000:2D08)
*/
#define	MMAP_SEGMENT	0x000002D0
#define	MMAP_ADDRESS	0x00002D00
#define	MMAP_SECTORS	0x0a

#define	MMAP_CODE	0xE820		/* int 0x15 code */
#define	MMAP_MAGIC_NUM	0x534D4150	/* for 0xE820 interrupt */

#endif
