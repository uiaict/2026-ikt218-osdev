extern interrupt_handler

global idt_flush
global isr0
global isr1
global isr2
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

section .text
bits 32

%macro ISR_NOERR 2
%1:
    push dword 0
    push dword %2
    jmp interrupt_common_stub
%endmacro

%macro IRQ 2
%1:
    push dword 0
    push dword %2
    jmp interrupt_common_stub
%endmacro

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

ISR_NOERR isr0, 0
ISR_NOERR isr1, 1
ISR_NOERR isr2, 2

IRQ irq0, 32
IRQ irq1, 33
IRQ irq2, 34
IRQ irq3, 35
IRQ irq4, 36
IRQ irq5, 37
IRQ irq6, 38
IRQ irq7, 39
IRQ irq8, 40
IRQ irq9, 41
IRQ irq10, 42
IRQ irq11, 43
IRQ irq12, 44
IRQ irq13, 45
IRQ irq14, 46
IRQ irq15, 47

interrupt_common_stub:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call interrupt_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8
    iret
