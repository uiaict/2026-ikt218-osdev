global isr0
global isr1
global isr2

extern isr_handler

isr0:
    cli                 ; temporarily disable interrupts
    pusha               ; save all registers on the stack
    push byte 0         ; push interrupt number
    call isr_handler    ; call C function
    add esp, 4          ; clean up the stack
    popa                ; restore registers
    sti                 ; re-enable interrupts
    iret                ; return from interrupt

isr1:
    cli
    pusha
    push byte 1
    call isr_handler
    add esp, 4
    popa               
    sti
    iret

isr2:
    cli
    pusha
    push byte 2
    call isr_handler
    add esp, 4
    popa
    sti
    iret