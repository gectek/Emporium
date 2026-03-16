//#include "io.h"
#include "debug.h"
#include "pmm.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "VGA_Text.h"
#include "multiboot.h"
#include "stdfun.h"
void kmain(multiboot_info_t *mbi, unsigned int magic){
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC("\nINVALID MAGIC NUMBER\n");
    }
    gdt_init();
    serial_init();

    //Initializing terminal
    terminal_init(main_term);
    
    //Initializing Interrupt descriptor table
    idt_init();
    
    //Initializing Page frame allocator
    pmm_init(mbi);

    /*uintptr_t addr = (uintptr_t)kalloc_frame();
    print("Memory allocated at %x\n", addr);
    uintptr_t addr2 = (uintptr_t)kalloc_frame();
    print("Memory allocated at %x\n", addr2);
    
    kfree_frame((void*)addr2);
    print("Free'd Address = %x\n", addr2);
    
    addr2 = (uintptr_t)kalloc_frame();
    print("Memory allocated at %x\n", addr2);*/
    
    while(1)
    {
        __asm__("hlt");
    }
}
