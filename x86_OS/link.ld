ENTRY(loader)               /*The name of entry loader*/

SECTIONS{
    . = 0x00100000;         /*The code should be stored after 1MB*/
    .text ALIGN (0x1000):   /*align at 4kb*/
    {
        *(.text)            /*all text  sections from all files*/
    }

    .rodata ALIGN (0x1000):
    {
        *(.rodata)          /*all read-only data from all files*/
    }

    .data ALIGN (0x1000):
    {
        *(.data)            /*all initialized data from all files*/
    }

    .bss ALIGN (0x1000):
    {
        *(COMMON)           /*all common sections from all files*/
        *(.bss)             /*all uninitialized data from all files*/
    }
}
