; global descriptor table (GDT) implementation


bits 32

CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10

global reloadSegments

section .rodata
align 8


; null segment descriptor
gdt_start:
    dq 0x0

; code segment descriptor
gdt_code:
    dw 0xFFFF        ; segment length
    dw 0x0000        ; base of the segment
    db 0x00          ; middle of the segment
    db 10011010b     ; access byte
    db 11001111b     ; flags 
    db 0x00          ; last of segment

; data segment descriptor
gdt_data:
    dw 0xFFFF        ; segment length
    dw 0x0000        ; base of the segment
    db 0x00          ; middle of the segment
    db 10010010b     ; access byte
    db 11001111b     ; flags 
    db 0x00          ; last of segment    

gdt_end:

section .rodata
align 8

; gdt descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1     ; size of gdt - 1
    dd gdt_start                 ; address (32 bit)

section .text

; Reloading segment registers
reloadSegments:
   lgdt [gdt_descriptor] 
  	
   ; Far jump to reload CS register containing code selector:
   jmp   CODE_SEGMENT:.reload_CS 
.reload_CS:
   ; Reload data segment registers:
   mov   ax, DATA_SEGMENT
   mov   ds, ax
   mov   es, ax
   mov   fs, ax
   mov   gs, ax
   mov   ss, ax
   
   ret

