bits 32

; Export ISR stubs so idt.c can register them in the IDT
global isr0
global isr1
global isr2
global lidt

; Import the shared C handler that all ISRs funnel into
extern isr_common_c

SECTION .text

; ISR0 - Division by zero exception
isr0:
    cli
    push dword 0        ; error code placeholder
    push dword 0        ; interrupt number
    call isr_common_c
    add  esp, 8
    sti
    iretd

; ISR1 - Debug exception
isr1:
    cli
    push dword 0
    push dword 1
    call isr_common_c
    add  esp, 8
    sti
    iretd

; ISR2 - Non-maskable interrupt (NMI)
isr2:
    cli
    push dword 0
    push dword 2
    call isr_common_c
    add  esp, 8
    sti
    iretd

; lidt wrapper
lidt:
    ; [esp+4] is return address
    mov eax, [esp + 4]
    lidt [eax]
    ret
