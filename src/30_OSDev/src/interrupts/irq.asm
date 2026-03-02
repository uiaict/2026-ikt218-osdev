; irq.asm - Assembly stubs for IRQ0-IRQ15

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

extern irq_handler

; Macro to define each IRQ stub
%macro IRQ_STUB 1
global irq%1
irq%1:
     cli                 ; Disable interrupts
    pusha               ; Save all general-purpose registers
    push byte %1        ; Push IRQ number to stack
    call irq_handler    ; Call C handler
    add esp, 4          ; Clean up stack (remove IRQ number)
    popa                ; Restore registers
    sti                 ; Enable interrupts
    iret                ; Return from interrupt
%endmacro

; Generate IRQ stubs
IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15