#include "irq.h"
#include "include/print.h"
#include "keyboard.h"
#include "include/io.h"

// The PIC has two chips - master and slave - each with two ports
// Command ports are used to configure the PIC
// Data ports are used to set which interrupts are masked (ignored)
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define VGA_CTRL_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

// End of Interrupt signal - must be sent to PIC after handling an IRQ
// so it knows we are done and can send the next one
#define PIC_EOI      0x20

void irq_init() {
    // Remap the PIC so IRQ0-15 map to interrupts 0x20-0x2F
    // This avoids collision with CPU exceptions at 0x00-0x1F

    // Start initialisation sequence
    outb(PIC1_COMMAND, 0x11);  // init master PIC
    outb(PIC2_COMMAND, 0x11);  // init slave PIC

    // Set vector offsets
    outb(PIC1_DATA, 0x20);     // master PIC: IRQ0 starts at interrupt 0x20
    outb(PIC2_DATA, 0x28);     // slave PIC:  IRQ8 starts at interrupt 0x28

    // Tell PICs how they are connected to each other
    outb(PIC1_DATA, 0x04);     // master: slave is on IRQ2
    outb(PIC2_DATA, 0x02);     // slave: its cascade identity

    // Set 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Unmask all IRQs (0x00 = all enabled)
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

// Called from irq_common_stub in irq.asm when a hardware interrupt fires
void irq_handler(uint32_t irq_no) {
    //debug green timer
    //if (irq_no == 0) {
    //    print_string("IRQ0: Timer", 0x02);
    //    print_newline();
    //}


    // IRQ1 is the PS/2 keyboard
    if (irq_no == 1) {
        keyboard_handler();
    }

    if (irq_no >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}
