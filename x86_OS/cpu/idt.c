#include "idt.h"
#include "pic.h"
#include "stdfun.h"
#include "timer.h"
#include "types.h"
#include "keyboard.h"
#include "paging.h"
#include "serial.h"
extern void *isr_stub_table[];
extern void *irq_stub_table[];
#define IDT_MAX_DESC 256
static bool vectors[IDT_MAX_DESC];
static idt_entry_t idt[IDT_MAX_DESC] __attribute__((aligned(0x10)));

static idtr_t idtr;

//An array for status of interrupts(exceptions and IRQs)
void *interrupt_handlers[IDT_MAX_DESC] = {0};
//A function for installing a handler
void install_int_handler(int intr, registers_t* (*handler)(registers_t *regs))
{
    interrupt_handlers[intr] = handler;
}
void install_exception_handler(int intr, void (*handler)(registers_t *regs))
{
    interrupt_handlers[intr] = handler;
}
//uninstalling handler
void uninstall_int_handler(int intr)
{
    interrupt_handlers[intr] = 0;
}

void exception_handler(registers_t *regs){
    print("I am Here!! : %d\n", regs->int_no);
    print("eip : %x\n", regs->eip);
    print("esp: %x\n", regs->esp);
    void (*handler)(registers_t *regs);
    if(interrupt_handlers[regs->int_no] != 0)
    {
        handler = interrupt_handlers[regs->int_no];

        //Execute
        handler(regs);
    }
    else if(regs->int_no == 13)
    {
        print("EXCEPTION 13: GPF\n");
        print("Error Code: %x\n", regs->err_code);
        while(1); // Halt
    }
    else {
    
    print("Unhandled Exception Occured: %d\n", regs->int_no);
     __asm__ volatile ("cli; hlt");
    }
}
registers_t *irq_handler(registers_t *regs)
{
    //print("Hardware Irq Fired: %d\n",regs.int_no);
    //uint8_t vector_eff = regs.int_no - 32;
    registers_t *(*handler)(registers_t *regs); 
    handler = interrupt_handlers[regs->int_no];
    registers_t* esp = regs;
    if(handler)
    {
        esp = handler(regs);
    }
    //Send EOI while entering to the handler,only in the beginning, further researches needed
    PIC_sendEOI(regs->int_no);
    return esp;
    
}
void idt_set_descriptor(uint32_t vector, void *isr, uint8_t flags){
    idt_entry_t *descriptor = &idt[vector];
    descriptor->isr_low = (uintptr_t)isr&0xFFFF; //lowest 16 bits
    descriptor->kernel_cs = 0x08;               //from GDT
    descriptor->reserved = 0;
    descriptor->attributes = flags;
    descriptor->isr_high = (uintptr_t)isr>>16;
}
void idt_init(){
    //setting up the idtr
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * (IDT_MAX_DESC - 1);
    if(idtr.limit == 0)
    {
        serial_write(SERIAL_COM1_BASE,"IDTR LIMIT IS 0\n");
        for(;;);
    }
    uint32_t vector;
    //disable_lapic();
    //remapping the PIC
    PIC_remap(0x20,0x28);
    
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    //Setting the entries of cpu exceptions
    for(vector=0;vector<32;vector++){

        idt_set_descriptor(vector,isr_stub_table[vector],0x8E);
        vectors[vector] = 1;
    }
    uint32_t irq_vec = vector;

    //Setting the entries of irq_interrupts
    for(vector=0;vector<16;vector++,irq_vec++)
    {
        idt_set_descriptor(irq_vec, irq_stub_table[vector], 0x8E);
        vectors[irq_vec] = 1;
    }
    timer_init();
    keyboard_init();
    pagefault_init();
    __asm__ volatile ("sti");
}
