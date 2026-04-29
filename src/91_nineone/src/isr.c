#include "isr.h"
#include "idt.h"

#include ""


static void print_interrupt_number(uint32_t n) {
    if (n == 0) {
        print("0");
    } else if (n == 1) {
        print("1");
    } else if (n == 2) {
        print("2");
    } else if (n == 3) {
        print("3");
    } else {
        print("unknown");
    }
}





void isr_handler(registers_t regs) {
    print("Interrupt triggered: ");
    print_interrupt_number(regs.int_no);
    print("\n");
}