; interrupt_stubs.asm
;
; Assembly entry points for all CPU exceptions (ISR 0-31) and hardware
; interrupts (IRQ 0-15, remapped to INT 0x20-0x2F).
;
; Each stub saves the full CPU state into a registers_t struct on the stack,
; switches to the kernel data segment, then calls the corresponding C
; dispatch function. On return the state is restored and iret resumes execution.
;
; ---- registers_t stack layout (from LOW to HIGH address) ----
;
;   [esp+0 ] ds         <- push ds (last push before call)
;   [esp+4 ] edi        \
;   [esp+8 ] esi         |
;   [esp+12] ebp         |  pusha pushes EAX first, EDI last
;   [esp+16] esp_dummy   |  (EDI ends up at the lowest address)
;   [esp+20] ebx         |
;   [esp+24] edx         |
;   [esp+28] ecx         |
;   [esp+32] eax        /
;   [esp+36] int_no     <- pushed by stub macro
;   [esp+40] err_code   <- pushed by stub macro (or CPU for some exceptions)
;   [esp+44] eip        \
;   [esp+48] cs          |  pushed automatically by the CPU
;   [esp+52] eflags     /

[BITS 32]

extern isr_dispatch   ; void isr_dispatch(registers_t *regs) in isr.c
extern irq_dispatch   ; void irq_dispatch(registers_t *regs) in irq.c

; ---------------------------------------------------------------
; Macro: ISR stub for exceptions that do NOT push an error code.
; We push a dummy 0 so the stack layout is always the same.
; ---------------------------------------------------------------
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0    ; dummy error code
    push dword %1   ; interrupt vector number
    jmp isr_common_stub
%endmacro

; ---------------------------------------------------------------
; Macro: ISR stub for exceptions that DO push an error code.
; The CPU already pushed the error code before we get here;
; we only push the interrupt number on top of it.
; ---------------------------------------------------------------
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push dword %1   ; interrupt vector number (error code already on stack)
    jmp isr_common_stub
%endmacro

; ---------------------------------------------------------------
; Macro: IRQ stub for hardware interrupts (remapped to 0x20-0x2F).
; %1 = IRQ line number (0-15)
; %2 = remapped interrupt vector (0x20 + %1)
; ---------------------------------------------------------------
%macro IRQ 2
global irq%1
irq%1:
    cli
    push dword 0    ; dummy error code
    push dword %2   ; remapped interrupt vector number
    jmp irq_common_stub
%endmacro

; -----------------------------------------------------------------------
; CPU Exception stubs (ISR 0-31)
;
; Which exceptions push an error code is an x86 architectural fact:
;   With error code:    8, 10, 11, 12, 13, 14, 17, 30
;   Without error code: everything else
; Getting this wrong silently corrupts the stack layout.
; -----------------------------------------------------------------------

ISR_NOERRCODE 0    ; #DE  Division Error
ISR_NOERRCODE 1    ; #DB  Debug
ISR_NOERRCODE 2    ;      Non-Maskable Interrupt
ISR_NOERRCODE 3    ; #BP  Breakpoint
ISR_NOERRCODE 4    ; #OF  Overflow
ISR_NOERRCODE 5    ; #BR  Bound Range Exceeded
ISR_NOERRCODE 6    ; #UD  Invalid Opcode
ISR_NOERRCODE 7    ; #NM  Device Not Available
ISR_ERRCODE   8    ; #DF  Double Fault           (error code always 0)
ISR_NOERRCODE 9    ;      Coprocessor Seg Overrun (obsolete)
ISR_ERRCODE   10   ; #TS  Invalid TSS
ISR_ERRCODE   11   ; #NP  Segment Not Present
ISR_ERRCODE   12   ; #SS  Stack-Segment Fault
ISR_ERRCODE   13   ; #GP  General Protection Fault
ISR_ERRCODE   14   ; #PF  Page Fault
ISR_NOERRCODE 15   ;      Reserved
ISR_NOERRCODE 16   ; #MF  x87 Floating-Point Exception
ISR_ERRCODE   17   ; #AC  Alignment Check
ISR_NOERRCODE 18   ; #MC  Machine Check
ISR_NOERRCODE 19   ; #XM  SIMD Floating-Point Exception
ISR_NOERRCODE 20   ; #VE  Virtualization Exception
ISR_NOERRCODE 21   ; #CP  Control Protection Exception
ISR_NOERRCODE 22   ;      Reserved
ISR_NOERRCODE 23   ;      Reserved
ISR_NOERRCODE 24   ;      Reserved
ISR_NOERRCODE 25   ;      Reserved
ISR_NOERRCODE 26   ;      Reserved
ISR_NOERRCODE 27   ;      Reserved
ISR_NOERRCODE 28   ; #HV  Hypervisor Injection Exception
ISR_NOERRCODE 29   ; #VC  VMM Communication Exception
ISR_ERRCODE   30   ; #SX  Security Exception
ISR_NOERRCODE 31   ;      Reserved

; -----------------------------------------------------------------------
; Hardware IRQ stubs (IRQ 0-15 → INT 0x20-0x2F after PIC remapping)
; -----------------------------------------------------------------------

IRQ  0, 0x20   ; PIT timer
IRQ  1, 0x21   ; PS/2 keyboard
IRQ  2, 0x22   ; Cascade (slave PIC)
IRQ  3, 0x23   ; COM2
IRQ  4, 0x24   ; COM1
IRQ  5, 0x25   ; LPT2
IRQ  6, 0x26   ; Floppy disk
IRQ  7, 0x27   ; LPT1 / spurious
IRQ  8, 0x28   ; RTC
IRQ  9, 0x29   ; Free / ACPI
IRQ 10, 0x2A   ; Free
IRQ 11, 0x2B   ; Free
IRQ 12, 0x2C   ; PS/2 mouse
IRQ 13, 0x2D   ; FPU / coprocessor
IRQ 14, 0x2E   ; Primary ATA
IRQ 15, 0x2F   ; Secondary ATA / spurious

; -----------------------------------------------------------------------
; Common ISR stub  (CPU exceptions 0-31)
; -----------------------------------------------------------------------
isr_common_stub:
    pusha                   ; Push EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
                            ; (EDI ends up at top/lowest address)

    mov ax, ds
    push eax                ; Save current data segment (as 32-bit)

    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                ; Pass pointer to registers_t as argument
    call isr_dispatch
    add esp, 4              ; Clean up the pushed argument

    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                    ; Restore EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX
    add esp, 8              ; Discard int_no and err_code
    iret                    ; Restores EIP, CS, EFLAGS (and ESP/SS if ring change)

; -----------------------------------------------------------------------
; Common IRQ stub  (hardware interrupts 0x20-0x2F)
; Identical to isr_common_stub but calls irq_dispatch instead.
; irq_dispatch sends the EOI to the PIC before returning.
; -----------------------------------------------------------------------
irq_common_stub:
    pusha

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_dispatch
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    iret
