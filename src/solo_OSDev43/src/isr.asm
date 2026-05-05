extern isr_handler

global isr0
global isr1
global isr2

isr0:
    push dword 0
    call isr_handler
    add esp, 4
    iret

isr1:
    push dword 1
    call isr_handler
    add esp, 4
    iret

isr2:
    push dword 2
    call isr_handler
    add esp, 4
    iret