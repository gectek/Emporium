global loader
extern kmain
MAGIC_NUMBER equ 0x1BADB002
FLAGS equ 0x3
CHECKSUM equ -(MAGIC_NUMBER+FLAGS)
KERNEL_STACK_SIZE equ 4092  ;kernel stack size
section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM
loader:
    ;mov esp, kernel_stack + KERNEL_STACK_SIZE
    mov edx, eax    ;Saving the MAGIC_NUM to edx, as eax will be overwritten
    
    ;Initialization for filling the first page_table(For booting)
    mov ecx, 1024           ;Setting the counter value as 1024

    mov edi, (BootPage_table - 0xC0000000) ;Starting from the address BootPage_table(base)

    mov eax, 0x03           ;Setting the Security bits
    
    ;Filling the table(Identity mapping till 4MB)
fill_table:                 ;Label for filling the table
    
    mov [edi], eax          ;Storing the value in eax to the address in edi
    add eax, 4096           ;Incrementing eax to get the next page address
    add edi, 4              ;Incrementing edi(index), (4 bytes = size of one entry)
    loop fill_table         
    
    ;Entering the PDE, (page_table addr)
    mov eax, (BootPage_table - 0xC0000000)
    or eax, 0x03            ;Setting the Security bits

    mov [(BootPage_directory - 0xC0000000)], eax   ;Mapping both higher half and starting addresses to the same page_table
    mov [(BootPage_directory - 0xC0000000) + 768*4], eax
    
    mov eax, (BootPage_directory - 0xC0000000)     ;Storing the address of page_directory in cr3 for MMU
    or eax, 0x03
    mov [(BootPage_directory - 0xC0000000) + 1023*4], eax
    
    mov eax, (BootPage_directory - 0xC0000000)
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000              ;Setting the 31st bit for enabling Paging
    mov cr0, eax
   
    ;Jumping to higher half
    lea eax, [higher_half]
    jmp eax


higher_half:
    mov esp, stack_top
    push edx 
    push ebx
    call kmain
.loop:
    jmp .loop
section .bss
; Align the stack to a 4-byte boundary for 32-bit CPU performance
align 16 
kernel_stack:           
    ;Reserving space for kernel
    resb KERNEL_STACK_SIZE
stack_top:

align 4096      ;Align the the page_directory space to 4KB boundary
BootPage_directory:
    resb 4096   ;Reserving space for Bootpage_directory
BootPage_table:
    resb 4096   ;Reserving space for Bootpage_table
