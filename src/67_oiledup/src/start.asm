global gdt_flush     ; Removed the leading underscore
extern gp            ; Removed the leading underscore

gdt_flush:
    lgdt [gp]        ; Load the GDT with our 'gp'
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2  ; Far jump to flush CS
flush2:
    ret              ; Returns back to the C code