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
    mov ax, 0x2B      ; TSS selector is 0x28 (5th entry * 8), but we need to set the RPL to 3? 
                      ; Wait: TSS is entry 5. 5 * 8 = 40 = 0x28. 
                      ; But it needs to be accessible from user mode? No, usually it's just 0x28.
                      ; However, some implementations use 0x2B (0x28 | 3).
                      ; Let's use 0x28 first.
    ltr ax            ; Load the TSS selector into the task register
    ret              ; Returns back to the C code
