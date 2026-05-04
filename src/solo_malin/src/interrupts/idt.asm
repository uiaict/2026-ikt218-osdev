[GLOBAL idt_flush]

; ==============================
; Load Interrupt Descriptor Table (IDT)
;
; Loads the IDT using the pointer
; passed from C into the IDTR register.
; ==============================

idt_flush:
    mov eax, [esp + 4]      ; Get pointer to idt_ptr (argument from C)
    lidt [eax]              ; Load IDT using lidt instruction
    ret                      ; Return to caller
