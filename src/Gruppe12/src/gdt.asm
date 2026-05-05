; Eksporterer funksjonen slik at C-koden kan kalle den
global gdt_flush

[section .text]

; Laster GDT inn i CPU-en og aktiverer nye segmenter
; Må være i assembler fordi C ikke har direkte tilgang til lgdt og segment-registre
gdt_flush:
    mov eax, [esp+4]   ; Hent adresse til gdt_ptr fra stack (første argument)
    lgdt [eax]         ; Last GDT inn i CPU-en sitt GDTR-register (Global Descriptor Table Register)
    
    ; Last inn segment-registre med nye verdier
    ; 0x10 = selector for GDT[2] (Data-segment, index 2 × 8 = 16 = 0x10)
    mov ax, 0x10
    mov ds, ax         ; Data segment register
    mov es, ax         ; Extra segment register
    mov fs, ax         ; Additional segment register
    mov gs, ax         ; Additional segment register
    mov ss, ax         ; Stack segment register (viktig for at stack skal fungere)
    
    ; Far jump for å laste inn CS (Code Segment) registeret
    ; CS kan IKKE lastes med "mov" - må bruke jump
    ; 0x08 = selector for GDT[1] (Code-segment, index 1 × 8 = 8 = 0x08)
    jmp 0x08:.flush
.flush:
    ret                ; Returner til C-koden