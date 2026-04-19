global gdt_flush

[section .text]
gdt_flush:
    mov eax, [esp+4]   ; Hent adresse til gdt_ptr fra stack
    lgdt [eax]         ; Last GDT inn i CPU-registret GDTR

    ; Last inn segmentregistrene med nye offset-verdier
    mov ax, 0x10       ; Data-segment (index 2 << 3)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump for å laste inn CS-registeret (Code-segment = index 1 << 3 = 0x08)
    jmp 0x08:.flush
.flush:
    ret