/*
** SCCS ID: @(#)x86pit.h	2.3        3/10/23
**
** File:    x86pit.h
**
** Author:  Warren R. Carithers
**
** Contributor: K. Reek
**
** Description: Definitions of constants and macros for the
**      Intel 8254 Programmable Interval Timer
**
*/

#ifndef _X86PIT_H_
#define _X86PIT_H_


/*
** Hardware timer (Intel 8254 Programmable Interval Timer)
**
** Control word layout:
**
**   Bit     7   6 | 5   4 | 3   2   1 | 0
**   Field  SC1 SC0|RW1 RW0|M2  M1  M0 |BCD
**
** SC  - select counter
** RW  - read/write
** M   - mode
** BCD - binary or BCD counter
*/

/* Frequency settings */
#define PIT_DEFAULT_TICKS_PER_SECOND  18  // actually 18.2065Hz
#define PIT_DEFAULT_MS_PER_TICK       (1000/PIT_DEFAULT_TICKS_PER_SECOND)
#define PIT_FREQUENCY                 1193182 // clock cycles/sec

/* Port assignments */
#define PIT_BASE_PORT         0x40    // I/O port for the timer
#define PIT_0_PORT            ( PIT_BASE_PORT )
#define PIT_1_PORT            ( PIT_BASE_PORT + 1 )
#define PIT_2_PORT            ( PIT_BASE_PORT + 2 )
#define PIT_CONTROL_PORT      ( PIT_BASE_PORT + 3 )

/* BCD field */
#define PIT_USE_DECIMAL       0x00    // 16-bit binary counter (default)
#define PIT_USE_BCD           0x01    // BCD counter

/* Timer modes */
#define PIT_MODE_0            0x00    // int on terminal count
#define PIT_MODE_1            0x02    // one-shot
#define PIT_MODE_2            0x04    // divide-by-N
#define PIT_MODE_3            0x06    // square-wave
#define PIT_MODE_4            0x08    // software strobe
#define PIT_MODE_5            0x0a    // hardware strobe

/* Timer 0 settings */
#define PIT_0_SELECT          0x00          // select timer 0
#define PIT_0_LOAD            0x30          // load LSB, then MSB
#define PIT_0_NDIV            PIT_MODE_2    // divide-by-N counter
#define PIT_0_SQUARE          PIT_MODE_3    // square-wave mode
#define PIT_0_ENDSIGNAL       0x00          // assert OUT at end of count

/* Timer 1 settings */
#define PIT_1_SELECT          0x40    // select timer 1
#define PIT_1_READ            0x30    // read/load LSB then MSB
#define PIT_1_RATE            0x06    // square-wave, for USART

/* Timer 2 settings */
#define PIT_2_SELECT          0x80    // select timer 1
#define PIT_2_READ            0x30    // read/load LSB then MSB
#define PIT_2_RATE            0x06    // square-wave, for USART

/* Timer read-back */
#define PIT_READBACK          0xc0    // perform a read-back
#define PIT_RB_NOT_COUNT      0x20    // don't latch the count
#define PIT_RB_NOT_STATUS     0x10    // don't latch the status
#define PIT_RB_CHAN_2         0x08    // read back channel 2
#define PIT_RB_CHAN_1         0x04    // read back channel 1
#define PIT_RB_CHAN_0         0x02    // read back channel 0
#define PIT_RB_ACCESS_MASK    0x30    // access mode field
#define PIT_RB_OP_MASK        0x0e    // oper mode field
#define PIT_RB_BCD_MASK       0x01    // BCD mode field

#endif
