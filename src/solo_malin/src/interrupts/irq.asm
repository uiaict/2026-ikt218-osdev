extern irq_handler

; ==============================
; IRQ stubs (hardware interrupts)
;
; Each IRQ N entry:
;  - Disables further interrupts
;  - Pushes dummy error code
;  - Pushes interrupt number
;  - Jumps to common IRQ handler stub
; ==============================

%macro IRQ 2
    global irq%1            ; Make label visible to linker (irq0, irq1, etc.)
    irq%1:
        CLI                 ; Disable interrupts to prevent nested interrupts
        PUSH LONG 0         ; Push dummy error code (for consistency with ISRs)
        PUSH LONG %2        ; Push interrupt number (mapped IDT entry)
        JMP irq_common_stub ; Jump to shared handler code
%endmacro

; Generate IRQ stubs for IRQ 0–15 (mapped to IDT entries 32–47)
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; ==============================
; Common IRQ handler stub
; This runs for ALL hardware interrupts
; ==============================


irq_common_stub:
    pusha               ; Save all general-purpose registers (EAX, EBX, etc.)
    mov ax, ds          ; Save current data segment
    PUSH eax            ; Push it onto stack

    ; Switch to kernel data segment (0x10)
    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    PUSH esp            ; Pass pointer to register stack (struct) to C handler
    CALL irq_handler    ; Call C function to handle interrupt

    ADD esp, 4          ; Clean up argument (esp pointer)
    POP ebx             ; Restore original data segment
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx

    POPA                ; Restore all general-purpose registers
    ADD esp, 8          ; Remove pushed error code + interrupt number
    STI                 ; Re-enable interrupts
    IRETD               ; Return from interrupt (restore EIP, CS, EFLAGS)