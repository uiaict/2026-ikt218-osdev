#include "isr.h" 
#include "util.h"
#include "screen.h"

const char *execption_messages[] = {
    "Devision by Zero",
    "Debug",
    "Non Maskable Interrupt", 
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds", 
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknow Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Macine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void isr_handler(struct  InterruptRegisters* r){

    write_string("ISR fired\n");

    //if (r->int_no == 3){
    //    write_string("INT 3 OK\n");
    //} else if (r->int_no == 4){
    //    write_string("INT 4 OK\n");
    //} else if (r->int_no == 0) {
    //    write_string("INT 0 Ok\n");
    //} else {
    //    write_string("Unexpected interrupt\n");
    //}

    //for (;;) {}

    if (r->int_no < 32){
        write_string((char *)execption_messages[r->int_no]);
        write_string("\n");

        char buf[4] = " 0";
        buf[0] = '0' + (r->int_no / 10);
        buf[1] = '0' + (r->int_no % 10);
        buf[2] = '\0';
        write_string("int_no  = ");
        write_string(buf);

        write_string("\nException! System Halted\n");
        for (;;) ;

    }

}

