extern isr_handler

; ==============================
; ISR stubs (CPU exceptions)
;
; Creates interrupt handler stubs
; for CPU exceptions and software
; interrupts before jumping to a
; shared common handler.
; ==============================

; ISR without CPU-provided error code
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        CLI                 ; Disable interrupts
        PUSH LONG 0         ; Push dummy error code
        PUSH LONG %1        ; Push interrupt number
        JMP isr_common_stub ; Jump to shared handler
%endmacro

; ISR with CPU-provided error code
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        CLI                 ; Disable interrupts
        PUSH LONG %1        ; Push interrupt number
        JMP isr_common_stub ; Jump to shared handler
%endmacro

; CPU exception handlers (0–31)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; Software interrupts / system calls
ISR_NOERRCODE 128
ISR_NOERRCODE 177


; ==============================
; Common ISR handler stub
;
; Saves CPU state, switches to
; kernel data segments, calls
; C-level isr_handler(), then
; restores state and returns.
; ==============================

isr_common_stub:
    pusha               ; Save all general-purpose registers
    mov ax, ds
    PUSH eax            ; Save current data segment

    MOV ax, 0x10        ; Load kernel data segment selector
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    PUSH esp            ; Pass pointer to interrupt register state
    CALL isr_handler

    ADD esp, 4          ; Remove function argument from stack
    POP ebx             ; Restore original data segment
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx

    POPA                ; Restore general-purpose registers
    ADD esp, 8          ; Remove interrupt number and error code
    STI                 ; Re-enable interrupts
    IRETD               ; Return from interrupt