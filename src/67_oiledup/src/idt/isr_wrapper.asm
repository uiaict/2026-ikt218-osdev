global isr0
global isr8
global isr14

extern interrupt_handler

section .text

isr0:
    push dword 0
    jmp isr_wrapper

isr8:
    push dword 8
    jmp isr_wrapper

isr14:
    push dword 14
    jmp isr_wrapper

isr_wrapper:
    pushad
    cld
    push dword [esp+32]
    call interrupt_handler
    add esp, 4
    popad
    add esp, 4
    iret