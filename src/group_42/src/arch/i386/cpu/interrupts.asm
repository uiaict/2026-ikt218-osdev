[extern isr_handler]
[extern irq_handler]
[extern syscall_handler]

isr_common_stub:
	pusha			; Pushes edi, esi, ebp, esp, ebx, edx, edx, eax

	mov ax, ds		; Lower 16-bits of eax = ds.
	push eax		; save the data segment descriptor

	mov ax, 0x10	; load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp    ; pass pointer to pushed registers to C handler
    cld
	call isr_handler
	add esp, 4 ; clean up esp arg

	pop eax			; reload the original data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa			; Pops edi, esi, ebp...
	add esp, 8		; Cleans up the pushed error code and pushed ISR number
	sti
	iret


irq_common_stub:
	pusha			; Pushes edi, esi, ebp, esp, ebx, edx, edx, eax

	mov ax, ds		; Lower 16-bits of eax = ds.
	push eax		; save the data segment descriptor

	mov ax, 0x10	; load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp
    cld
	call irq_handler
	add esp, 4

	pop eax			; reload the original data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa			; Pops edi, esi, ebp...
	add esp, 8		; Cleans up the pushed error code and pushed ISR number
	sti
	iret			; pops 5 things at once, CS, EIP, EFLAGS, SS, and ESP


; defines all CPU defined ISRs
; isr_no_err_stub  N: ISR N does not push  an error code.
; isr_err_stub M: ISR M pushes an error code.
; irq O, V: IRQ O maps to vector V (0-15 -> 32-47)

; isr_stub_table and irq_stub_table are used by C IDT setup
%macro isr_err_stub 1
isr_stub_%+%1:
    CLI
    PUSH BYTE %1
    JMP isr_common_stub
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    CLI
    PUSH BYTE 0
    PUSH BYTE %1
    JMP isr_common_stub
%endmacro

%macro irq 2
irq_stub_%+%1:
    CLI
    PUSH BYTE 0
    PUSH BYTE %2
    JMP irq_common_stub
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

%assign i 0
%rep 16
irq i, 32 + i
%assign i i+1
%endrep

; isr_stub_table[n] == address of isr_stub_n, for IDT entry n (0–31)
global isr_stub_table
isr_stub_table:
    ; entries 0-31 correspond to CPU exceptions and NMI/IRQ
    %assign i 0
    %rep    32
        DD isr_stub_%+i
    %assign i i+1
    %endrep
; irq_stub_table[n] == address of irq_stub_n, for IDT entry (32 + n)
global irq_stub_table
irq_stub_table:
    ; entries 0-15 correspond to PIC IRQ 0-15 (vectors 32-47)
    %assign i 0
    %rep    16
        DD irq_stub_%+i
    %assign i i+1
    %endrep

; Syscall entry for 0x80 (Linux‑style: eax=sySCALLno, ebx=arg1, ecx=arg2, etc.).
; The C handler syscall_handler receives a pointer to registers so it can build
; a syscall_args_t struct and dispatch via syscall_table.
global syscall_stub
syscall_stub:
    CLI
    PUSH 0 ; Error code
    PUSH 0x80 ; ISR number
    PUSHA

    mov ebx, esp ; ebx = pointer to pushed registers for syscall_handler
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push ebx
    cld
    call syscall_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    POPA
    add esp, 8
    iret
