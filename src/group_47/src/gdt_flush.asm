[bits 32]
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]  ; Hent parameteren fra C-koden (pekeren til gdt_ptr)
    lgdt [eax]          ; Last GDT-pekeren inn i CPU-ens GDTR-register

    ; Nå må vi oppdatere alle segment-registrene til å peke på det nye Data-segmentet.
    ; I vår init_gdt() er Data-segmentet nr 2, altså offset 0x10 (2 * 8 bytes).
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Til slutt gjør vi et "Far Jump" for å oppdatere CS (Code Segment).
    ; Code-segmentet vårt er nr 1, altså offset 0x08 (1 * 8 bytes).
    jmp 0x08:.flush_cs

.flush_cs:
    ret