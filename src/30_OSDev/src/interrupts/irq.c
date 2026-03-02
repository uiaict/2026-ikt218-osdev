#include "irq.h"
#include "../terminal.h"  // For printing messages
#include "io.h"        // For outb function (I/O port write)

#define PIC1 0x20 // Master PIC command port
#define PIC2 0xA0 // Slave PIC command port
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 1)


// Function to remap the PIC so IRQs don't conflict with CPU exceptions
void pic_remap() {
    
    outb(PIC1_COMMAND, 0x11);// Start initialization sequence in cascade mode
    outb(PIC2_COMMAND, 0x11);

    // Set vector offsets
    outb(PIC1_DATA, 0x20); // Master PIC IRQs start at 32
    outb(PIC2_DATA, 0x28); // Slave PIC IRQs start at 40

    
    outb(PIC1_DATA, 0x04); // Tell Master PIC about Slave at IRQ2
    outb(PIC2_DATA, 0x02); // Tell Slave its cascade identity

    
    outb(PIC1_DATA, 0x01);  // Set PICs to 8086/88 mode
    outb(PIC2_DATA, 0x01);
}

// General IRQ handler called from assembly ISR
void irq_handler(int irq_num) {
    // Print IRQ number to the screen
    terminal_write("IRQ triggered: ");
    char buf[4];
    itoa(irq_num, buf, 10); // Convert number to string
    terminal_write(buf);
    terminal_write("\n");

    // Send End of Interrupt (EOI) signal to PICs
    if (irq_num >= 40) { // If IRQ came from Slave PIC
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20); // Always send to Master PIC
}

// Simple itoa function for decimal numbers
void itoa(int value, char* str, int base) {
    char *ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    if (value == 0) { *str++='0'; *str='\0'; return; }

    while (value) {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? tmp_value + '0' : tmp_value + 'A' - 10;
        value /= base;
    }
    *ptr--='\0';

    while(ptr1 < ptr) { 
        tmp_char = *ptr; 
        *ptr-- = *ptr1; 
        *ptr1++ = tmp_char; 
    }
}
