/*
** Author: Ezequiel Salas
** Parallel port driver
** Communicate with dot matrix printer
**
*/

#include "cio.h"
#include "common.h"
#include "lib.h"
#include "support.h"
#include "x86arch.h"
#include "x86pic.h"

#include "pio.h"

//could be 0x278
//or could be 0x3BC

#define PORT_ONE 0xD000
#define PORT_TWO 0xD010

//Maybe manually build out a table of chars and special chars
//to make processing easier

//Later clean up code to match our coding style

//Pin 10-13 are status pins from Printer
//Pin 32 is error pin



//May not even work anymore

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    __outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((__inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}
//May not even work anymore

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function){
    uint16_t r0 = pciConfigReadWord(bus,device,function,0);
    return r0;

}
//May not even work anymore

uint16_t checkDevice(uint8_t bus, uint8_t device){
    uint8_t function = 0;
    uint8_t vendorID;
    vendorID = getVendorID(bus, device, function);
    if (vendorID == 0xFFFF) return 0;
    return (vendorID);
    
}
//May not even work anymore
void bruteForce(void){
    uint16_t bus;
    uint8_t device;
    __cio_puts("TESING\n");
    __delay(100000);

    while (true){
        __delay(5000);
    }
    for (bus = 0; bus < 256; bus++){
        for (device = 0; device < 32; device++){
            checkDevice(bus,device);
        }
    }
}

/**
 * Name: process(char)
 * 
 * Description: TODO. Process keyboard inputs and send it to the parallel port.
 * Perform necessary timing actions to print everything.
*/

int process(char buffer[]){

    return 0;
}

/**
 * Name establish_handshake(void)
 * 
 * Description: Communicate via the second I/O port for the parallel port.
 * Wait until BUSY signal or PAPER OUT is zero and then send test bytes to printer.
 * If the ERROR signal is received output status code and return
*/
int establish_handshake(void){
    //Wait until printer is online and ready to process data
    //Low busy means we are online
    //Low acknowledge means we are ready for the next block
    __outb(PORT_TWO+2, 4);
    __delay(50);

    unsigned char status = __inb(PORT_TWO+1);
    __cio_printf("Status at init: %08x\n", status);
     while((status & 0x80)){
            __cio_puts("BUSY\n");
            __cio_printf("Status at busy: %08x\n", status);
             status = __inb(PORT_TWO+1);
             __delay(100);
    }
    //All good to accept data

    //REDO ALL OF THIS FOR BETTER STUFF
    //check for Paper out signal
    while (status & 0x20){
        __cio_puts("No Paper\n");
        __cio_printf("Status at paper error: %08x\n", status);
        __delay(100);
        status = __inb(PORT_TWO+1);
    }
    //Check for any error codes before starting
    if ( (status & 0x8) ){
        //General Error
        //For now just return and exit
        __cio_puts("General Error\n");
        __cio_printf("Status at general error: %08x\n", status);
        return 2;
    }

    __cio_puts("All good to accept data\n");


    __outb(PORT_TWO, 't');

    unsigned char strobe = __inb(PORT_TWO+2);
    __outb(PORT_TWO+2, strobe | 1);
    __delay(2);
    __outb(PORT_TWO+2, strobe);

 	while ((__inb( PORT_TWO+1 ) & 0x80) )
 	{
        __delay(3);
 	}

    __outb(PORT_TWO, 'e');
    strobe = __inb(PORT_TWO+2);
    __outb(PORT_TWO+2, strobe | 1);
    __delay(2);
    __outb(PORT_TWO+2, strobe);

 	while ((__inb( PORT_TWO+1 ) & 0x80) )
 	{
        __delay(3);
 	}
    __outb(PORT_TWO, 's');
    strobe = __inb(PORT_TWO+2);
    __outb(PORT_TWO+2, strobe | 1);
    __delay(2);
    __outb(PORT_TWO+2, strobe);

 	while ((__inb( PORT_TWO+1 ) & 0x80) )
 	{
        __delay(3);
 	}
    __outb(PORT_TWO, 't');
    strobe = __inb(PORT_TWO+2);
    __outb(PORT_TWO+2, strobe | 1);
    __delay(2);
    __outb(PORT_TWO+2, strobe);


 	while ((__inb( PORT_TWO+1 ) & 0x80) )
 	{
        __delay(10);
 	}


    //All good start doing work
    return 0;
}

/**
 * Name: pio_init(void)
 * 
 * Description: Calls the appropriate functions to initialize a connection
 * to printer. Afterwards if it works it would call a process function to
 * handle keyboard input to send to the printer
 * 
 * 
*/
int pio_init(void) {
    __cio_puts("PIO INIT\n");
    //bruteForce();
    establish_handshake();
    //process();
    return 0;
}