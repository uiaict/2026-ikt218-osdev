#include "interrupts.h"

/** Loads the interrupt descriptor table
 * \warning Assumes intel syntax
 */
void load_idt(struct idt_pointer idt_pointer) {
    // __asm__ __volatile__ (".intel_syntax noprefix\r\n");
    __asm__ __volatile__ ("lidt %0\r\n" : : "m"(idt_pointer));
    // __asm__ __volatile__ (".att_syntax prefix\r\n");
}

struct idt_gate create_idt_gate(uint32_t offset, uint16_t selector, uint8_t attributes)
{
    struct idt_gate a;
    a.low_offset = (offset);
    a.hi_offset = (offset >> 16);
    a.selector = selector;
    a.attributes = attributes;
    a.RESERVED_DO_NOT_USE = 0;

    return a;
}

uint8_t create_idt_attributes(bool present, int8_t ring, uint8_t type) {
    uint8_t a = 0;
    if (present) { a = 0b10000000; }
    else if (!present) { a = 0; }
    a = a | (ring << 5);
    a = a | type;
    return a;
}

// Helper to write to I/O ports
static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

/** Helper to add a tiny delay for older hardware compatibility 
 * \todo Is this needed?
*/
static void io_wait() {
    outb(0x80, 0);
}

void pic_remap(int offset1, int offset2) {
    uint8_t a1, a2;

    // 1. Save masks
    a1 = __builtin_ia32_inb(0x21);
    a2 = __builtin_ia32_inb(0xA1);

    // 2. Start initialization sequence (ICW1)
    outb(0x20, 0x11); 
    io_wait();
    outb(0xA0, 0x11);
    io_wait();

    // 3. Set Vector Offsets (ICW2)
    outb(0x21, offset1); // Usually 0x20 (32)
    io_wait();
    outb(0xA1, offset2); // Usually 0x28 (40)
    io_wait();

    // 4. Tell Master PIC there is a slave PIC at IRQ2 (ICW3)
    outb(0x21, 0x04);
    io_wait();
    // 5. Tell Slave PIC its cascade identity (ICW3)
    outb(0xA1, 0x02);
    io_wait();

    // 6. Set mode to 8086/88 (ICW4)
    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();

    // 7. Restore masks (or leave at 0 to enable all)
    outb(0x21, a1);
    outb(0xA1, a2);
}