global isr0
global isr8
global isr14
global isr32
global isr33

extern interrupt_handler

section .text

; --- Exceptions WITHOUT an error code: push dummy 0 to keep stack uniform ---
isr0:
    push dword 0        ; dummy error code
    push dword 0        ; interrupt number
    jmp isr_common_stub

isr32:
    push dword 0
    push dword 32
    jmp isr_common_stub

isr33:
    push dword 0
    push dword 33
    jmp isr_common_stub

; --- Exceptions WITH an error code: CPU already pushed it, just push int number ---
isr8:
    push dword 8        ; error code already on stack from CPU
    jmp isr_common_stub

isr14:
    push dword 14
    jmp isr_common_stub

; --- Uniform stack layout at this point:
;     [EFLAGS][CS][EIP][error_code][int_num]  <- ESP
isr_common_stub:
    pushad
    cld

    push dword [esp+32]     ; int_num is 8 regs * 4 bytes above ESP
    call interrupt_handler
    add esp, 4              ; remove argument

    ; Only send EOI to PIC for hardware IRQs (int >= 32)
    mov eax, [esp+32]
    cmp eax, 32
    jl .no_eoi
    mov al, 0x20
    out 0x20, al            ; send EOI to master PIC
    cmp eax, 40
    jl .no_eoi
    mov al, 0x20
    out 0xA0, al            ; send EOI to slave PIC too (IRQ8-15)
.no_eoi:
    popad
    add esp, 8              ; skip BOTH int_num AND error_code
    iret