
/**
 * Name: process(char)
 * 
 * Description: TODO. Process keyboard inputs and send it to the parallel port.
 * Perform necessary timing actions to print everything.
*/
int process(char buffer[]);

/**
 * Name establish_handshake(void)
 * 
 * Description: Communicate via the second I/O port for the parallel port.
 * Wait until BUSY signal or PAPER OUT is zero and then send test bytes to printer.
 * If the ERROR signal is received output status code and return
*/

int establish_handshake(void);

/**
 * Name: pio_init(void)
 * 
 * Description: Calls the appropriate functions to initialize a connection
 * to printer. Afterwards if it works it would call a process function to
 * handle keyboard input to send to the printer
 * 
 * 
*/
int pio_init(void);

