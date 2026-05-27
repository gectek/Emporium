ENTRY(loader)               /*The name of entry loader*/

SECTIONS{
    . = 0x00100000;         /*The code should be stored after 1MB*/
    .multiboot.data : {
        *(.multiboot.data)
    }

    . = 0xC0100000;
    .text : AT (ADDR(.text) - 0xC0000000)
    {
        *(.text)            /*all text  sections from all files*/
    }
    . = ALIGN(0x1000);
    .rodata : AT (ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)          /*all read-only data from all files*/
    }
    . = ALIGN(0x1000);
    .data : AT (ADDR(.data) - 0xC0000000)
    {
        *(.data)            /*all initialized data from all files*/
    }
    . = ALIGN(0x1000);
    .bss : AT (ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)           /*all common sections from all files*/
        *(.bss)             /*all uninitialized data from all files*/
        endkernel = .;
    }
}
