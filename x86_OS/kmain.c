//#include "io.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "VGA_Text.h"
void kmain(){
    gdt_init();
    serial_init();
    terminal_init(main_term);
    idt_init();
    while(1)
    {
        __asm__("hlt");
    }
}
