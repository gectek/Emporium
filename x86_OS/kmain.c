//#include "io.h"
//#include "VGA_Text.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "stdfun.h"

void kmain(){
    gdt_init();
    serial_init();
    terminal_init();
    print("Hello World %d\n",3);
    idt_init();
}
