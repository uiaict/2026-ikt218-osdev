global isr0, isr1, isr2
global irq0, irq1
global idt_load

extern pit_tick_increment
extern terminal_write
extern keyboard_handler

[section .text]

idt_load:
    mov eax, [esp+4]
    lidt [eax]
    sti
    ret

isr0:
    pusha
    push dword 0
    push dword 0
    call isr_common_stub
    add esp, 8
    popa
    iret

isr1:
    pusha
    push dword 1
    push dword 0
    call isr_common_stub
    add esp, 8
    popa
    iret

isr2:
    pusha
    push dword 2
    push dword 0
    call isr_common_stub
    add esp, 8
    popa
    iret

isr_common_stub:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    
    cmp eax, 0
    je .isr0_msg
    cmp eax, 1
    je .isr1_msg
    cmp eax, 2
    je .isr2_msg
    jmp .done
    
.isr0_msg:
    push msg_isr0
    call terminal_write
    add esp, 4
    jmp .done
.isr1_msg:
    push msg_isr1
    call terminal_write
    add esp, 4
    jmp .done
.isr2_msg:
    push msg_isr2
    call terminal_write
    add esp, 4
    
.done:
    mov esp, ebp
    pop ebp
    ret

msg_isr0: db "ISR0", 0
msg_isr1: db "ISR1", 0
msg_isr2: db "ISR2", 0

irq0:
    pusha
    push byte 32
    call irq_handler_common
    add esp, 4
    mov al, 0x20
    out 0x20, al
    call pit_tick_increment
    popa
    iret

irq1:
    cli                    ; Disable interrupts FØR vi leser
    pusha
    
    ; Les scancode med en gang
    in al, 0x60
    push eax
    
    ; Kall handler
    call keyboard_handler
    add esp, 4
    
    ; Send EOI TIDLIG
    mov al, 0x20
    out 0x20, al
    
    popa
    sti                    ; Re-enable interrupts
    iret

irq_handler_common:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    
    cmp eax, 32
    je .irq0_msg
    cmp eax, 33
    je .irq1_msg
    jmp .done
    
.irq0_msg:
    push msg_irq0
    call terminal_write
    add esp, 4
    jmp .done
.irq1_msg:
    ; IKKE skriv "Key pressed" for hver gang - det spammer
    ; push msg_irq1
    ; call terminal_write
    ; add esp, 4
    
.done:
    mov esp, ebp
    pop ebp
    ret

msg_irq0: db "IRQ0", 0
msg_irq1: db "IRQ1", 0