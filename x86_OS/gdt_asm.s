global gdt_flush        ;allow the kmain to call this function
gdt_flush:
    ;1) load the GDT
    mov eax, [esp+4]    ;moving the first argument(ie, struct gdt_ptr )
    lgdt [eax]          ;loading the gdt
    ;2) Reload Data Segments (DS,ES,FS,GS,SS)
    ;0x10 is the offset to our data segment(2*8 = 16 = 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;3)Reload code segment(CS)
    ;For updating CS, we must do a Far Jump
    ;offset = 0x08
    jmp 0x08:.flush
.flush:
    ret                 ;return to C code
