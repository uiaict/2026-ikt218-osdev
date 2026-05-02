#include "irq.h"
#include "util.h"

void *irq_routines[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

void irq_install_handler (int irq, void (*handler)(struct  InterruptRegisters *r)){
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq){
    irq_routines[irq] = 0;
}

void irq_handler(struct InterruptRegisters* r){
    void (*handler)(struct InterruptRegisters *r);

    handler = irq_routines[r->int_no - 32];

    if (handler) {
        handler(r);
    }

    if (r->int_no >= 40){
            outPortB(0xA0, 0x20);
    }

    outPortB(0x20, 0x20);
}

