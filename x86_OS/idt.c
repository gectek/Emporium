#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "types.h"
#include "keyboard.h"
extern void *isr_stub_table[];
extern void *irq_stub_table[];
#define IDT_MAX_DESC 256
static bool vectors[IDT_MAX_DESC];
__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESC];

static idtr_t idtr;

//An array for status of irqs
void *irq_routines[16] = 
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
    };
//A function for installing a handler
void install_irq_handler(int irq, void (*handler)(registers_t regs))
{
    irq_routines[irq] = handler;
}
//uninstalling handler
void uninstall_irq_handler(int irq)
{
    irq_routines[irq] = 0;
}



void exception_handler(registers_t regs){
     for(;;)
     __asm__ volatile ("cli; hlt");
}
void irq_handler(registers_t regs)
{
    uint8_t vector_eff = regs.int_no - 32;
    void (*handler)(registers_t regs); 
    handler = irq_routines[vector_eff];

    if(handler)
    {
        handler(regs);
    }
    //Send EOI while entering to the handler,only in the beginning, further researches needed
    PIC_sendEOI(regs.int_no);

    
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
    uint32_t vector;

    //remapping the PIC
    PIC_remap(0x20,0x28);

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
    timer_install();
    IRQ_clear_mask(0);
    keyboard_init();
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}
