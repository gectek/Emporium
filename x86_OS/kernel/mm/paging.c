#include "paging.h"
#include "idt.h"
#include "types.h"
#include "debug.h"
#include "pmm.h"
#include "stdfun.h"
void pagefault_handler(registers_t *regs)
{
    volatile uintptr_t fault_addr;
    uintptr_t phys_addr;
    //1) Find out the address,flags and error code
    __asm__ volatile("mov %%cr2, %0":"=r"(fault_addr)::);
    uint32_t ErrCode = regs->err_code;
    uint32_t flags = 0x07;
    uintptr_t alligned_virt_addr = fault_addr & 0xFFFFF000;
    
    print("The fault address is : %x , %d , %s\n", fault_addr,fault_addr,fault_addr);
    if((ErrCode & 1<<2) == 0)
    {
        panic("Kernel Panic: Page fault","paging.c",15);
    }
    if((ErrCode & 1) == 0) //If the condition for allocation is true
    {
        phys_addr = (uintptr_t)kalloc_frame();      //Getting the phys_addr of a free frame
        map_page((void*)phys_addr, (void*)alligned_virt_addr, flags);   //Mapping the physical address to virtual_addr

    }

}
void map_page(void *phys_addr, void *virtual_addr, unsigned int flags)
{
    uintptr_t pd_index = (uintptr_t)virtual_addr >> 22;
    //The last 10 bits are pd index
    uintptr_t pt_index = (uintptr_t)virtual_addr >> 12 & 0x3FF;

    uintptr_t *pd = (uintptr_t*)0xFFFFF000;
    //Recursive paging for accessing the address of page_directory

    uintptr_t *pt = (uintptr_t*)(0xFFC00000+ (0x1000*pd_index));
    //Same for accessing the page_table
    
    if((pd[pd_index] & 1) == 0)
    {
        //panic("Trying to access an address which is already in use","paging.h",14);
        uintptr_t new_table_physaddr = (uintptr_t)kalloc_frame();
        pd[pd_index] = new_table_physaddr | 0x07;
        
        for(int i=0; i<1024; i++)
        {
            pt[i] = 0;
        }
    }
     if(pt[pt_index] & 1)
    {
        print("I was here\n");
        panic("Trying to access an address which is already in use","paging.h",16);
        //In future updates : have to change this to custom bit action(Overwriting)
        //pt[pt_index] = ((uintptr_t)phys_addr) | (flags & 0xFFF) | 0x01; //For now just panic
    }

    //Entering the new entry to the page_table
    pt[pt_index] = ((uintptr_t)phys_addr) | (flags & 0xFFF) | 0x01; //last 1 for setting the present bit
    //TLB flushing 
    tlb_flush(virtual_addr);
}
void unmap_page(void *virtual_addr)
{
    uintptr_t virt = (uintptr_t)virtual_addr;
    uintptr_t pd_index = virt >> 22;    //top 10 bits
    uintptr_t pt_index = (virt >> 12)& 0x3FF;   //middle 10 bits
    
    //Accessing the page table
    uintptr_t *pt = (uintptr_t*)(0xFFC00000 + pd_index*0x1000);
    
    //clear the entry
    pt[pt_index] = 0;
    
    //flush tlb
    tlb_flush(virtual_addr);
}
void pagefault_init()
{
    install_exception_handler(14, pagefault_handler);
}
void tlb_flush(void *virtual_addr)
{
    __asm__ volatile("invlpg (%0)"::"r"(virtual_addr):"memory");
}
