global irq_handler_stub

extern irq_handler

section .text

%macro IRQ_STUB 1
global irq%1
irq%1:
    CLI
    PUSH BYTE 0       ; dummy error code
    PUSH BYTE %1      ; IRQ number
    JMP irq_common_stub
%endmacro

; IRQ0-15
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

irq_common_stub:
    PUSHA

    MOV EAX, [ESP + 32]   ; skip past the 8 registers PUSHA pushed (8 x 4 = 32 bytes) to get IRQ number
    PUSH EAX              ; push it as the argument to irq_handler
    CALL irq_handler
    ADD ESP, 4            ; clean up the argument we just pushed

    POPA
    ADD ESP, 8            ; clean up IRQ number and dummy error code
    STI
    IRET