; based on https://wiki.osdev.org/GDT_Tutorial#The_LDT  - using protected mode

[GLOBAL gdt_flush]      ; Make function visible to linker 

; ==============================
; Load Global Descriptor Table
;
; Loads the GDT (GDTR) from the
; pointer passed from C and then
; reloads segment registers to use
; the new descriptors.
; ==============================

gdt_flush:

    mov eax, [esp + 4]  ; Get pointer to gdt_ptr 
    lgdt [eax]          ; Load GDT using pointer

    ; Reload segment register with new GDT selectors

    mov ax, 0x10        ; Selector for data segment 
    mov ds, ax          ; Set Data segment 
    mov es, ax          ; Set Extra Segment 
    mov fs, ax          ; Set FS segment
    mov gs, ax          ; Set GS segment
    mov ss, ax          ; Set stack
    
    ; Far jump to reload CS with new code segment selector (0x08)
    jmp 0x08:return

return
    ret


