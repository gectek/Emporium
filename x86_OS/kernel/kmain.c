#include "MultiTasking/task.h"
#include "mm/paging.h"
#include "pic.h"
#include "../drivers/keyboard.h"
#include "shell.h"
//#include "io.h"
#include "debug.h"
#include "mm/heap.h"
#include "pmm.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "VGA_Text.h"
#include "multiboot.h"
#include "stdfun.h"
#include "paging.h"
#include "MultiTasking/scheduler.h"
extern uint32_t BootPage_directory;
extern pcb_t *task_list;
extern volatile int kb_head;
extern volatile int kb_tail;
extern volatile char kb_buffer[];
void task1() {
    while(1)
    {
        print("TASK_1   ");
    }
}
void task2() {
    while(1) {
        print("TASK_2    ");
        //serial_write(SERIAL_COM1_BASE,"TASK2 "); 
    }
}

void kmain(multiboot_info_t *phys_mbi_pointer, unsigned int magic){
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC("\nINVALID MAGIC NUMBER\n");
    }
    serial_init();
    gdt_init();
    terminal_init(main_term);
    //disable_lapic();
    idt_init();
    //Initializing terminal
    //terminal_init(main_term);
    
    
    //Initializing Page frame allocator
    //First shifting the mbi pointer to the higher half
    multiboot_info_t *mbi = (multiboot_info_t *)((uint32_t)phys_mbi_pointer + 0xC0000000);
    pmm_init(mbi);
    uintptr_t *pd = (uintptr_t *)PAGE_DIR_VIRTUAL;
    //pd[0] = 0;
    
    /*
        * TODO: Remove identity mapping.
        * Currently I am having a small issue in kvm enabled environment in qemu.
    */
    heap_init();
    scheduler_init();
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    pcb_t *t1 = create_task(task1, cr3);
    pcb_t *t2 = create_task(task2, cr3);
    //add_task(t1);
    //add_task(t2);
    //add_task((void*)task_reaper);
while(1) {
    if(kb_head != kb_tail)
        {
            char c = kb_buffer[kb_tail];
            kb_tail = (kb_tail + 1)%KB_BUFFER_SIZE;
            cli_input_handler(c);
        }
    __asm__ volatile("hlt");
}
}
