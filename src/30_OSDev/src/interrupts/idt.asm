global idt_flush

idt_flush:
    mov eax, [esp+4]   ; get parameter (pointer to idt_ptr)
    lidt [eax]         ; load IDT into CPU
    ret