global isr_stub_table
global irq_stub_table
isr_common_stub:
    ;1. Save all General purpose registers
    pusha       ;pushes edi,esi,ebp,esp,ebx,edx,ecx,eax onto stack
    ;2. Save segment register
    mov ax, ds  ;mov ds -> lower 16bits of eax
    push eax    ;save data segment register
    ;3. Load Kernel Data segment
    mov ax, 0x10;load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;4. Call C code
    call exception_handler
    ;5. Restore Segment Registers
    pop eax     ;Reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;6. Restore general purpose registers
    popa        ;pops all general purpose registers
    ;7. Clean up the stack
    add esp,8
    ;8. Return from interrupt
    iret        ;pops CS,EIP,EFLAGS,SS,ESP
irq_common_stub:
    ;1) Save all General purpose registers
    pusha   ;pushes edi,esi,ebp,esp,ebx,edx,ecx,eax onto the stack
    ;2) Save segment registers
    mov ax, ds  ;pushes ds->lower 16 bits of eax
    push eax    ;save data segment register
    ;3) Load Kernel Data segment
    mov ax, 0x10;load the  kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;4) Call C handler
    call irq_handler
    ;5) restore Segment registers
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;6) restore general purpose registers
    popa    ;pops GP regs
    ;7) Clean up the stack
    add esp, 8
    ;8) Return from interrupt
    iret;pops CS,EIP,EFLAGS,SS,ESP

%macro isr_err_stub 1
isr_stub_%+%1:
    cli             ;disabling all interrupts
                    ;cpu will automatically push the error code
    push byte %1    ;push the interrupt number(1st argument)
    jmp isr_common_stub
%endmacro

%macro isr_noerr_stub 1
isr_stub_%+%1:
    cli
    push byte 0     ;pushing a dummy error code
    push byte %1
    jmp isr_common_stub
%endmacro

%macro IRQ 2
irq%+%1:
    cli
    push byte 0     ;pushing dummy code
    push byte %2    ;pushing vector
    jmp irq_common_stub
%endmacro    
extern exception_handler
isr_noerr_stub 0
isr_noerr_stub 1
isr_noerr_stub 2
isr_noerr_stub 3
isr_noerr_stub 4
isr_noerr_stub 5
isr_noerr_stub 6
isr_noerr_stub 7
isr_err_stub 8
isr_noerr_stub 9
isr_err_stub 10
isr_err_stub 11
isr_err_stub 12
isr_err_stub 13
isr_err_stub 14
isr_noerr_stub 15
isr_noerr_stub 16
isr_err_stub 17
isr_noerr_stub 18
isr_noerr_stub 19
isr_noerr_stub 20
isr_noerr_stub 21
isr_noerr_stub 22
isr_noerr_stub 23
isr_noerr_stub 24
isr_noerr_stub 25
isr_noerr_stub 26
isr_noerr_stub 27
isr_noerr_stub 28
isr_noerr_stub 29
isr_err_stub 30
isr_noerr_stub 31
extern irq_handler
IRQ     0,  32
IRQ     1,  33
IRQ     2,  34
IRQ     3,  35
IRQ     4,  36
IRQ     5,  37
IRQ     6,  38
IRQ     7,  39
IRQ     8,  40
IRQ     9,  41
IRQ     10, 42
IRQ     11, 43
IRQ     12, 44
IRQ     13, 45
IRQ     14, 46
IRQ     15, 47


isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep
irq_stub_table:
%assign i 0
%rep 16
    dd irq%+i
%assign i i+1
%endrep

