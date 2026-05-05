; IDT loader and interrupt stubs for the 32-bit i386 kernel.

; Make idt_load visible to C code in idt.c.
global idt_load

; Make the three CPU exception stubs visible to idt.c.
global isr0
global isr1
global isr2

; Make all sixteen hardware IRQ stubs visible to idt.c.
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

; C function called after the assembly stub has saved the CPU state.
extern interrupt_handler

; Put the loader and stubs in the executable code section.
section .text

; Assemble all instructions as 32-bit protected-mode code.
bits 32

; void idt_load(struct idt_pointer* idt_pointer)
idt_load:
    ; The first cdecl argument is at [esp + 4].
    mov eax, [esp + 4]

    ; lidt loads IDTR from the 6-byte structure pointed to by EAX.
    lidt [eax]

    ; Return to C after the CPU knows where the IDT lives.
    ret

; CPU exception stubs. These interrupts do not push an error code.
isr0:
    ; Push a fake error code so every frame has the same shape.
    push dword 0
    ; Push the interrupt vector number for divide-by-zero / int 0x0.
    push dword 0
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

isr1:
    ; Push a fake error code so every frame has the same shape.
    push dword 0
    ; Push the interrupt vector number for debug / int 0x1.
    push dword 1
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

isr2:
    ; Push a fake error code so every frame has the same shape.
    push dword 0
    ; Push the interrupt vector number for NMI / int 0x2.
    push dword 2
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

; Hardware IRQ stubs. The PIC is remapped so IRQ0 starts at vector 32.
irq0:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 32, which is remapped IRQ0, the timer.
    push dword 32
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq1:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 33, which is remapped IRQ1, the keyboard.
    push dword 33
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq2:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 34, which is remapped IRQ2.
    push dword 34
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq3:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 35, which is remapped IRQ3.
    push dword 35
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq4:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 36, which is remapped IRQ4.
    push dword 36
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq5:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 37, which is remapped IRQ5.
    push dword 37
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq6:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 38, which is remapped IRQ6.
    push dword 38
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq7:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 39, which is remapped IRQ7.
    push dword 39
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq8:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 40, which is remapped IRQ8.
    push dword 40
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq9:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 41, which is remapped IRQ9.
    push dword 41
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq10:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 42, which is remapped IRQ10.
    push dword 42
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq11:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 43, which is remapped IRQ11.
    push dword 43
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq12:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 44, which is remapped IRQ12.
    push dword 44
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq13:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 45, which is remapped IRQ13.
    push dword 45
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq14:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 46, which is remapped IRQ14.
    push dword 46
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

irq15:
    ; Push a fake error code; hardware IRQs do not provide one.
    push dword 0
    ; Push vector 47, which is remapped IRQ15.
    push dword 47
    ; Jump to the shared register-saving code.
    jmp interrupt_common_stub

interrupt_common_stub:
    ; Save all general-purpose registers.
    pusha

    ; Save the current segment registers as 32-bit values for the C frame.
    ; DS is saved first, but after all four pushes ESP points at GS.
    ; Clear EAX so the upper 16 bits are not leftover data.
    xor eax, eax
    ; Copy the 16-bit DS selector into AX.
    mov ax, ds
    ; Push the cleaned 32-bit value onto the interrupt frame.
    push eax
    ; Save ES so the interrupted code's value can be restored later.
    ; Clear EAX so the upper 16 bits are not leftover data.
    xor eax, eax
    ; Copy the 16-bit ES selector into AX.
    mov ax, es
    ; Push the cleaned 32-bit value onto the interrupt frame.
    push eax
    ; Save FS so the interrupted code's value can be restored later.
    ; Clear EAX so the upper 16 bits are not leftover data.
    xor eax, eax
    ; Copy the 16-bit FS selector into AX.
    mov ax, fs
    ; Push the cleaned 32-bit value onto the interrupt frame.
    push eax
    ; Save GS so the interrupted code's value can be restored later.
    ; Clear EAX so the upper 16 bits are not leftover data.
    xor eax, eax
    ; Copy the 16-bit GS selector into AX.
    mov ax, gs
    ; Push the cleaned 32-bit value onto the interrupt frame.
    push eax

    ; Load the kernel data selector before calling C code.
    ; 0x10 is the data selector installed by the GDT code.
    mov ax, 0x10
    ; Use the kernel data segment for normal C data accesses.
    mov ds, ax
    ; Keep ES consistent with DS.
    mov es, ax
    ; Keep FS consistent with DS.
    mov fs, ax
    ; Keep GS consistent with DS.
    mov gs, ax

    ; Pass a pointer to the saved register frame as the first C argument.
    ; ESP currently points at the beginning of struct interrupt_frame.
    push esp
    ; Let C decide what to do with this interrupt or IRQ.
    call interrupt_handler
    ; Remove the one pointer argument pushed before the call.
    add esp, 4

    ; Restore segment registers in reverse order.
    ; Restore GS from the top saved segment value.
    pop eax
    mov gs, ax
    ; Restore FS.
    pop eax
    mov fs, ax
    ; Restore ES.
    pop eax
    mov es, ax
    ; Restore DS last.
    pop eax
    mov ds, ax

    ; Restore general-purpose registers.
    popa

    ; Drop interrupt_number and error_code from the stack.
    add esp, 8

    ; Return from the interrupt and restore EIP, CS, and EFLAGS.
    iret
