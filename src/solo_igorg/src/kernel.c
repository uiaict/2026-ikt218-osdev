#include <libc/stddef.h>
#include <libc/stdint.h>
#include <gdt.h>
#include <terminal.h>
#include <idt.h>
#include <irq.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static uint16_t* const VGA_BUFFER = (uint16_t*) VGA_MEMORY;

static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0x0F;

static uint16_t vga_entry(unsigned char character, uint8_t color)
{
    return (uint16_t) character | (uint16_t) color << 8;
}

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = 0x0F;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putchar(char character)
{
    if (character == '\n') {
        terminal_column = 0;
        terminal_row++;

        if (terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }

        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    VGA_BUFFER[index] = vga_entry(character, terminal_color);

    terminal_column++;

    if (terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;

        if (terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_write(const char* data)
{
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

void main(void)
{
    /*
     * Initializes GDT before using interrupts
     */
    gdt_initialize();

    /*
     * Initializes terminal output so kernel can print out status messages.
     */
    terminal_initialize();

    terminal_write("UiA OS\n");
    terminal_write("Assignment 2: Hello World output works\n");
    terminal_write("Hello World\n\n");

    /*
     * Initializes and loads IDT.
     */
    terminal_write("Assignment 3: Initializing IDT...\n");
    idt_initialize();
    terminal_write("IDT initialized\n");

    /*
     * Software interrupt test, used to verify that IDT and ISR stubs work.
     */
    terminal_write("\nTesting software interrupts...\n");

    terminal_write("Triggering interrupt 0x00...\n");
    __asm__ volatile ("int $0x00");

    terminal_write("Triggering interrupt 0x03...\n");
    __asm__ volatile ("int $0x03");

    terminal_write("Triggering interrupt 0x04...\n");
    __asm__ volatile ("int $0x04");

    terminal_write("Software interrupt test completed\n");

    /*
     * Initializes IRQ support.
     * Remaps PIC and registers IRQ0-IRQ15 in IDT.
     */
    terminal_write("\nInitializing IRQ support...\n");
    irq_initialize();
    terminal_write("IRQ support initialized\n");

    /*
     * IRQ vector test.
     * These software-triggered IRQ vectors verify that the IRQ stubs are
     * registered correctly after PIC remapping.
     * Comment these out when testing real keyboard IRQ1 handler, because
     * IRQ1 test does not come from actual keyboard hardware.
     */
    
    terminal_write("\nTesting IRQ vectors...\n");

    terminal_write("Triggering IRQ0 vector 0x20...\n");
    __asm__ volatile ("int $0x20");

    terminal_write("Triggering IRQ1 vector 0x21...\n");
    __asm__ volatile ("int $0x21");

    terminal_write("Triggering IRQ15 vector 0x2F...\n");
    __asm__ volatile ("int $0x2F");

    terminal_write("IRQ vector test completed\n");

    /*
     * Enables hardware interrupts so that the keyboard can generate IRQ1.
     */
    terminal_write("\nKeyboard logger ready. Type on keyboard:\n");

    __asm__ volatile ("sti");

    /*
     * Halt CPU while waiting for interrupts.
     * Keyboard input will wake the CPU through IRQ1.
     */
    while (1) {
        __asm__ volatile ("hlt");
    }
}
