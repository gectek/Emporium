//#include "io.h"
#include "debug.h"
#include "pmm.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "VGA_Text.h"
#include "multiboot.h"
#include "stdfun.h"
#include "paging.h"
void kmain(multiboot_info_t *phys_mbi_pointer, unsigned int magic){
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC("\nINVALID MAGIC NUMBER\n");
    }
    serial_init();
    gdt_init();
    idt_init();
    //Initializing terminal
    terminal_init(main_term);
    
    
    //Initializing Page frame allocator
    //First shifting the mbi pointer to the higher half
    multiboot_info_t *mbi = (multiboot_info_t *)(phys_mbi_pointer + 0xC0000000);
    pmm_init(mbi);
    
    uintptr_t *pd = (uintptr_t *)PAGE_DIR_VIRTUAL;
    pd[0] = 0;
    
    gdt_init();
    idt_init();
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
