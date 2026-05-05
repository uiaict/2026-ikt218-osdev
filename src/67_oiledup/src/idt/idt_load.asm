global idt_load

idt_load:
    mov eax, [esp+4]    ; Get the address for idtr from the stack
    lidt [eax]
    ret
