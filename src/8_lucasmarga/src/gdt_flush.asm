; gdt_flush.asm (NASM) - load GDTR and reload segment registers for 32-bit protected mode
; System V / GCC i386 cdecl calling convention:
;   gdt_load(const gdt_ptr_t* gdtr)
; argument at [esp + 4]

BITS 32
GLOBAL gdt_load

gdt_load:
    ; Load new GDT
    mov eax, [esp + 4]
    lgdt [eax]

    ; Reload data segment registers with DATA selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS with CODE selector (0x08)
    jmp 0x08:flush_cs

flush_cs:
    ret