; idt_flush.asm
; Loads the IDT pointer into the IDTR register using the lidt instruction.
;
; Unlike gdt_flush, no segment registers need to be reloaded after lidt.
; The CPU simply updates IDTR. Interrupts remain disabled until the caller
; explicitly executes sti.
;
; The C variable `ip` (of type idt_ptr_t) is defined in idt.c and holds
; the 6-byte IDTR value (2-byte limit + 4-byte base) that lidt reads.

[BITS 32]

global idt_flush   ; expose to C as  extern void idt_flush(void)
extern ip          ; idt_ptr_t ip  from idt.c

idt_flush:
    lidt [ip]   ; Load our IDT pointer into IDTR
    ret
