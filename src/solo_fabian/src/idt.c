/* The IDT gates need the kernel code selector from the GDT. */
#include <gdt.h>

/* Public declaration for idt_init(). */
#include <idt.h>

/* Local fixed-width integer and size types. */
#include <libc/stdint.h>

/* Interrupt handlers print status and keyboard characters to the VGA terminal. */
#include <terminal.h>

/* PIT counts timer IRQs for sleep functions. */
#include <pit.h>

/* The IDT has one entry for each possible x86 interrupt vector. */
#define IDT_ENTRY_COUNT 256

/* 0x8E means present, ring 0, 32-bit interrupt gate. */
#define IDT_INTERRUPT_GATE 0x8E

/* The two 8259 PIC chips are controlled through these I/O ports. */
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

/* End-of-interrupt command sent to the PIC after handling an IRQ. */
#define PIC_EOI 0x20

/* PIC initialization control words. */
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

/* Hardware IRQs are remapped away from CPU exceptions to vectors 32..47. */
#define IRQ_BASE 32

/* One packed 8-byte IDT gate in the exact format the CPU expects. */
struct idt_entry {
    /* Lower 16 bits of the ISR function address. */
    uint16_t offset_low;

    /* Code segment selector from the GDT. */
    uint16_t selector;

    /* Always zero for a 32-bit interrupt gate. */
    uint8_t zero;

    /* Gate type, privilege level, and present bit. */
    uint8_t type_attr;

    /* Upper 16 bits of the ISR function address. */
    uint16_t offset_high;
} __attribute__((packed));

/* The small pointer structure consumed by the lidt instruction. */
struct idt_pointer {
    /* Size of the whole IDT minus one, as required by lidt. */
    uint16_t limit;

    /* Linear address where the IDT begins. */
    uint32_t base;
} __attribute__((packed));

/* Register snapshot built by isr.asm before calling into C. */
struct interrupt_frame {
    /* Segment registers saved by isr.asm before switching to kernel data segments. */
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    /* General-purpose registers saved by the pusha instruction in isr.asm. */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    /* Values pushed by each individual ISR/IRQ stub before the common handler. */
    uint32_t interrupt_number;
    uint32_t error_code;

    /* Values pushed automatically by the CPU when the interrupt happened. */
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

/* Assembly helper that runs lidt for the pointer we build here. */
extern void idt_load(struct idt_pointer* idt_pointer);

/* CPU exception stubs. */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);

/* Hardware IRQ stubs after PIC remapping. */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

/* Storage for all IDT entries. */
static struct idt_entry idt[IDT_ENTRY_COUNT];

/* Storage for the pointer passed to lidt. */
static struct idt_pointer idt_ptr;

/* Simple keyboard input buffer for translated ASCII characters. */
static char keyboard_buffer[256];

/* Number of characters currently stored in keyboard_buffer. */
static size_t keyboard_buffer_length;

/* US QWERTY scancode set 1 lookup table for normal key presses. */
static const char keyboard_scancode_to_ascii[128] = {
    /* 0x00 - 0x07 */
    0, 27, '1', '2', '3', '4', '5', '6',
    /* 0x08 - 0x0F */
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    /* 0x10 - 0x17 */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    /* 0x18 - 0x1F */
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    /* 0x20 - 0x27 */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    /* 0x28 - 0x2F */
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    /* 0x30 - 0x37 */
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    /* 0x38 - 0x3F */
    0, ' ', 0, 0, 0, 0, 0, 0,
    /* 0x40 - 0x47 */
    0, 0, 0, 0, 0, 0, 0, '7',
    /* 0x48 - 0x4F */
    '8', '9', '-', '4', '5', '6', '+', '1',
    /* 0x50 - 0x57 */
    '2', '3', '0', '.', 0, 0, 0, 0,
    /* 0x58 - 0x5F */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x60 - 0x67 */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x68 - 0x6F */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x70 - 0x77 */
    0, 0, 0, 0, 0, 0, 0, 0
};

/* Write one byte to an x86 I/O port. */
static void outb(uint16_t port, uint8_t value)
{
    /* Put value in AL and port in DX/immediate, then execute the outb instruction. */
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Read one byte from an x86 I/O port. */
static uint8_t inb(uint16_t port)
{
    /* The byte read from the port will be returned through AL. */
    uint8_t value;

    /* Read one byte from the given I/O port into value. */
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));

    /* Return the port byte to the C caller. */
    return value;
}

/* Small I/O delay used while programming the old PIC hardware. */
static void io_wait(void)
{
    /* Port 0x80 is traditionally used as a tiny delay after PIC commands. */
    outb(0x80, 0);
}

/* Fill one IDT gate with the address of an assembly ISR stub. */
static void idt_set_gate(uint8_t vector, uint32_t handler)
{
    /* Store the low half of the assembly stub address. */
    idt[vector].offset_low = (uint16_t)(handler & 0xFFFF);

    /* Tell the CPU to run the handler using the kernel code segment. */
    idt[vector].selector = GDT_KERNEL_CODE_SELECTOR;

    /* This byte is reserved and must be zero. */
    idt[vector].zero = 0;

    /* Mark the entry as a present 32-bit interrupt gate. */
    idt[vector].type_attr = IDT_INTERRUPT_GATE;

    /* Store the high half of the assembly stub address. */
    idt[vector].offset_high = (uint16_t)((handler >> 16) & 0xFFFF);
}

/* Remap IRQ0..IRQ15 from vectors 0..15 to vectors 32..47. */
static void pic_remap(void)
{
    /* Save the existing interrupt masks so remapping does not unexpectedly unmask lines. */
    uint8_t pic1_mask = inb(PIC1_DATA);
    uint8_t pic2_mask = inb(PIC2_DATA);

    /* Start initialization on the master PIC and say that ICW4 will follow. */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* Start initialization on the slave PIC and say that ICW4 will follow. */
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* Remap master IRQ0..IRQ7 to interrupt vectors 32..39. */
    outb(PIC1_DATA, IRQ_BASE);
    io_wait();

    /* Remap slave IRQ8..IRQ15 to interrupt vectors 40..47. */
    outb(PIC2_DATA, IRQ_BASE + 8);
    io_wait();

    /* Tell the master PIC that the slave is connected on IRQ2. */
    outb(PIC1_DATA, 4);
    io_wait();

    /* Tell the slave PIC its cascade identity is line 2. */
    outb(PIC2_DATA, 2);
    io_wait();

    /* Put the master PIC into 8086/88 mode. */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    /* Put the slave PIC into 8086/88 mode. */
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /* Restore the masks that were active before remapping. */
    outb(PIC1_DATA, pic1_mask);
    outb(PIC2_DATA, pic2_mask);
}

/* Keep every IRQ masked except IRQ1, so keyboard input is readable on screen. */
static void pic_enable_timer_and_keyboard_irq(void)
{
    /* 0xFC masks every master IRQ except bit 0 and bit 1. */
    outb(PIC1_DATA, 0xFC);

    /* 0xFF masks all slave PIC IRQs, so IRQ8..IRQ15 stay quiet for now. */
    outb(PIC2_DATA, 0xFF);
}

/* Tell the PIC that the current hardware interrupt has been handled. */
static void pic_send_eoi(uint32_t interrupt_number)
{
    /* IRQ8..IRQ15 come from the slave PIC, so the slave needs an EOI too. */
    if (interrupt_number >= IRQ_BASE + 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    /* Every hardware IRQ also needs an EOI sent to the master PIC. */
    outb(PIC1_COMMAND, PIC_EOI);
}

/* Store and print one translated keyboard character. */
static void keyboard_store_character(char character)
{
    /* Keep a copy of typed characters while there is room in the buffer. */
    if (keyboard_buffer_length < sizeof(keyboard_buffer)) {
        keyboard_buffer[keyboard_buffer_length] = character;
        keyboard_buffer_length++;
    }

    /* Echo the translated key to the VGA terminal. */
    terminal_putchar(character);
}

/* Read the keyboard controller, translate the scancode, and print the result. */
static void keyboard_handle_irq(void)
{
    /* The PS/2 controller places the latest keyboard scancode on port 0x60. */
    uint8_t scancode = inb(0x60);

    /* Scancodes with bit 7 set are key releases; this logger prints key presses only. */
    if ((scancode & 0x80) != 0) {
        return;
    }

    /* Translate the scancode to ASCII using the lookup table above. */
    char character = keyboard_scancode_to_ascii[scancode];

    /* Zero means the key is not printable in this simple layout. */
    if (character != 0) {
        keyboard_store_character(character);
    }
}

/* Common C interrupt handler called by every assembly stub in isr.asm. */
void interrupt_handler(struct interrupt_frame* frame)
{
    /* Interrupts below 32 are CPU exceptions or software interrupts. */
    if (frame->interrupt_number < IRQ_BASE) {
        terminal_write("ISR triggered: ");
        terminal_putchar((char)('0' + frame->interrupt_number));
        terminal_putchar('\n');
        return;
    }

    /* Vector 32 is IRQ0, the programmable interval timer. */
    if (frame->interrupt_number == IRQ_BASE) {
        pit_handle_tick();
    /* Vector 33 is IRQ1, the PS/2 keyboard. */
    } else if (frame->interrupt_number == IRQ_BASE + 1) {
        keyboard_handle_irq();
    /* The other IRQ stubs are installed even though they only print a generic message. */
    } else {
        terminal_write("IRQ triggered\n");
    }

    /* Hardware IRQs must be acknowledged or the PIC will not deliver the next one. */
    pic_send_eoi(frame->interrupt_number);
}

/* Build the IDT, load it with lidt, and prepare hardware IRQ delivery. */
void idt_init(void)
{
    /* lidt wants the table size minus one, just like lgdt did for the GDT. */
    idt_ptr.limit = (uint16_t)(sizeof(idt) - 1);

    /* Point lidt at the first byte of the IDT array. */
    idt_ptr.base = (uint32_t)&idt;

    /* Install three software/CPU interrupt handlers for the assignment demo. */
    idt_set_gate(0, (uint32_t)isr0);
    idt_set_gate(1, (uint32_t)isr1);
    idt_set_gate(2, (uint32_t)isr2);

    /* Install handlers for all 16 remapped hardware IRQ lines. */
    idt_set_gate(IRQ_BASE + 0, (uint32_t)irq0);
    idt_set_gate(IRQ_BASE + 1, (uint32_t)irq1);
    idt_set_gate(IRQ_BASE + 2, (uint32_t)irq2);
    idt_set_gate(IRQ_BASE + 3, (uint32_t)irq3);
    idt_set_gate(IRQ_BASE + 4, (uint32_t)irq4);
    idt_set_gate(IRQ_BASE + 5, (uint32_t)irq5);
    idt_set_gate(IRQ_BASE + 6, (uint32_t)irq6);
    idt_set_gate(IRQ_BASE + 7, (uint32_t)irq7);
    idt_set_gate(IRQ_BASE + 8, (uint32_t)irq8);
    idt_set_gate(IRQ_BASE + 9, (uint32_t)irq9);
    idt_set_gate(IRQ_BASE + 10, (uint32_t)irq10);
    idt_set_gate(IRQ_BASE + 11, (uint32_t)irq11);
    idt_set_gate(IRQ_BASE + 12, (uint32_t)irq12);
    idt_set_gate(IRQ_BASE + 13, (uint32_t)irq13);
    idt_set_gate(IRQ_BASE + 14, (uint32_t)irq14);
    idt_set_gate(IRQ_BASE + 15, (uint32_t)irq15);

    /* Move PIC interrupts away from vectors 0..15, which are reserved for CPU exceptions. */
    pic_remap();

    /* Let the keyboard IRQ through while keeping noisy timer and unused IRQs masked. */
    pic_enable_timer_and_keyboard_irq();

    /* Load the finished IDT into the CPU's IDTR register. */
    idt_load(&idt_ptr);
}
