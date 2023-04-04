Make read command public so it can be read by printer code.
Everything else can be private.

Keep track of pointer in PCB since there are 4 bytes at the end of the struct.
Add this at the end of the pcb and then adjust the bytes.