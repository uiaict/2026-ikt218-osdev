[GLOBAL gdt_flush] ; Export the function so it can be called from C code (gdt.c)

gdt_flush:
    mov eax, [esp+4]  ; Corresponds to the first argument (pointer to GDT): gdt_flush(&gdt_ptr); Since the arguments are 
                      ; passed on the stack, we need to access it using esp. 1st argument is located at [esp+4] bc [esp] is the return address.
                      ; It is then loaded into eax(a registry) for use in the lgdt instruction
    lgdt [eax]        ; A special instruction to load the GDT register with the address of our GDT pointer. This tells the CPU where our GDT is located in memory.

    ; Set segment registers to 2nd entry (Data Segment) = 0x10 = 2 * 8 (size of each GDT entry)
    mov ax, 0x10      
    mov ds, ax ; Data Segment
    mov es, ax ; Extra Segment
    mov fs, ax ; FS Segment
    mov gs, ax ; GS Segment
    mov ss, ax ; Stack Segment

    ; Set Code Segment to 1st entry = 0x08 = 1 * 8
    jmp 0x08:.flush

.flush:
    ret ; Return from the function, control goes back to the caller (kernel_main)