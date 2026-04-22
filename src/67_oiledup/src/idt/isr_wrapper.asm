.globl   isr_wrapper
.align   4

isr0:
    push 0
    jmp isr_wrapper

isr8:
    push 8
    jmp isr_wrapper

isr14:
    push 14
    jmp isr_wrapper

isr_wrapper:
    pushad
    cld    ; C code following the sysV ABI requires DF to be clear on function entry
    call interrupt_handler
    popad
    add esp, 4   ; remove interrupt number from stack
    iret