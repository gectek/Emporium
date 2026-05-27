#ifndef IDT_H
#define IDT_H
#include "types.h"
#include "pic.h"
typedef struct{
    //data segment selector
    uint32_t ds;
    //Pushed by pusha
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
    //interrupt # and error code
    uint32_t int_no,err_code;
    //Pushes automatically by the cpu upon interrupt
    uint32_t eip,cs,eflags,usersp,ss;

}__attribute__((packed)) registers_t;
typedef struct{
    uint16_t isr_low;   //lower 16 bits of the address of ISR
    uint16_t kernel_cs; //the GDT segment selector that the CPU will load into CS before calling ISR
    uint8_t reserved;   //set to zero
    uint8_t attributes;//Type and attributes(gate types,dpl and p fields)
    uint16_t isr_high;

}__attribute__((packed)) idt_entry_t;

typedef struct{         //struct for the idt register for storing the idt address
    uint16_t limit;
    uint32_t base;
}__attribute__((packed)) idtr_t;

__attribute__((noreturn))
void exception_handler(registers_t regs);           //Declaring the exception handler

void irq_handler(registers_t regs);
void idt_set_descriptor(uint32_t vector, void *isr, uint8_t flags);  //Function for setting an entry in idt
void idt_init(void);
void install_int_handler(int intr, void (*handler)(registers_t regs));
void uninstall_int_handler(int intr);
#endif
