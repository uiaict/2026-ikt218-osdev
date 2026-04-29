global loadGDT        ; make loadGDT visible to C
global reloadSegments ; make reloadSegments visible to C

section .text         ; everything below is executable code

loadGDT:
    MOV EAX, [ESP + 4]  ; ESP points to the stack, +4 skips the return address to get our argument
    LGDT [EAX]          ; tell the CPU where the GDT is
    RET

reloadSegments:
    ; We can't load CS directly with MOV, so we use a far jump
    ; The far jump loads 0x08 (our code segment selector) into CS
    JMP 0x08:.reload_CS

.reload_CS:
    ; Load 0x10 (our data segment selector) into all data segment registers
    ; We have to go through AX first since you can't load segment registers directly
    MOV AX, 0x10
    MOV DS, AX   ; data segment
    MOV ES, AX   ; extra segment
    MOV FS, AX   ; general purpose segment
    MOV GS, AX   ; general purpose segment
    MOV SS, AX   ; stack segment
    RET