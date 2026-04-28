; Export gdt_load so C code can call it like a normal function.
global gdt_load

; Put this code in the executable text section.
section .text

; This kernel is currently built as 32-bit i386 code.
bits 32

; void gdt_load(struct gdt_pointer* gdt_pointer)
;
; lgdt loads the new table, but the CPU keeps using the old cached segment
; descriptors until each segment register is reloaded. The far jump reloads CS.
gdt_load:
    ; In the 32-bit cdecl calling convention, the first argument is at [esp + 4].
    ; Here it is the address of the packed gdt_pointer structure from gdt.c.
    mov eax, [esp + 4]

    ; Load GDTR from the 6-byte structure pointed to by EAX:
    ;   2 bytes: GDT size minus one
    ;   4 bytes: GDT base address
    lgdt [eax]

    ; 0x10 is GDT entry 2, the kernel data descriptor.
    mov ax, 0x10

    ; Reload DS so normal data accesses use the new data descriptor.
    mov ds, ax

    ; Reload ES, used by some string/memory instructions.
    mov es, ax

    ; Reload FS even though this tiny kernel does not use it yet.
    mov fs, ax

    ; Reload GS for the same reason: keep all data segment registers consistent.
    mov gs, ax

    ; Reload SS so stack accesses also use the new data descriptor.
    mov ss, ax

    ; 0x08 is GDT entry 1, the kernel code/text descriptor.
    ; A far jump changes CS and flushes the old cached code-segment descriptor.
    jmp 0x08:.reload_code_segment

.reload_code_segment:
    ; Return to the C caller after CS has been refreshed.
    ret
