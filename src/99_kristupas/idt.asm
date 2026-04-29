global loadIDT   ; make loadIDT visible to C

; ISR stubs - each one needs to be visible to C so we can pass the address to idt_set_gate()
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5

; External C function that handles the interrupt
extern isr_handler

section .text

loadIDT:
    MOV EAX, [ESP + 4]  ; ESP+4 skips the return address on the stack to reach the first argument
    LIDT [EAX]          ; tell the CPU where the IDT is
    RET

; The macro does the repetitive work - every ISR stub looks the same
%macro ISR_NOERR 1
isr%1:
    CLI                  ; disable interrupts while we handle this one
    PUSH BYTE 0          ; push a dummy error code (some exceptions push one, some don't)
    PUSH BYTE %1         ; push the interrupt number so the C handler knows which one fired
    JMP isr_common_stub  ; jump to shared handler
%endmacro

; These CPU exceptions don't push an error code, so i send dummy 0
ISR_NOERR 0   ; divide by zero
ISR_NOERR 1   ; debug
ISR_NOERR 2   ; non-maskable interrupt
ISR_NOERR 3   ; breakpoint
ISR_NOERR 4   ; overflow
ISR_NOERR 5   ; bound range exceeded

isr_common_stub:
    PUSHA                ; save all general purpose registers (like putting tools down safely)
    CALL isr_handler     ; call our C function with the stack as context
    POPA                 ; restore all registers
    ADD ESP, 8           ; clean up the two values we pushed (error code + interrupt number)
    STI                  ; re-enable interrupts
    IRET                 ; return from interrupt (like RET but also restores EFLAGS and CS)