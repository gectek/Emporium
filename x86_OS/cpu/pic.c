#include "pic.h"
#include "io.h"

void PIC_remap(uint32_t offset1, uint32_t offset2){
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  //starts the initialization sequence in cascade mode
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);                   //ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, offset2);                   //ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 1<<IRQ_CASCADE);            //ICW3: Tell the Master PIC, there is a slave PIC at IRQ2
    io_wait();
    outb(PIC2_DATA, 2);                         //ICW3: Tell the Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);                 //ICW4: Set the PICs to use 8086 mode
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /*//Unmask both PICs
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
    */
    PIC_disable();
}
void PIC_disable(){
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}
void PIC_sendEOI(uint8_t irq){                  //Need to call this at the end of every hardware interrupt handlers
    if(irq >= 40)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
void IRQ_set_mask(uint8_t IRQLine){
    uint16_t port;
    uint8_t value;
    if(IRQLine<8){
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = inb(port) | (1 << IRQLine);
    outb(port, value);
}
void IRQ_clear_mask(uint8_t IRQLine){
    uint16_t port;
    uint8_t value;
    if(IRQLine<8)
        port = PIC1_DATA;
    else{
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = inb(port) & ~(1 << IRQLine);
    outb(port, value);
}
/*
static uint16_t __pic_get_irq_reg(int ocw3){
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8| inb(PIC1_COMMAND));
}
uint16_t pic_get_irr(){
    return __pic_get_irq_reg(PIC_READ_IRR);
}
uint16_t pic_get_isr(){
    return __pic_get_irq_reg(PIC_READ_ISR);
}
*/
void disable_lapic()
{
    uintptr_t hi, lo;
    uintptr_t APIC_BASE_MSR = 0x1B;
    uintptr_t APIC_ENABLE_BIT = (1 << 11);
    
    //Read the current state of APIC in MSR
    cpu_rdmsr(APIC_BASE_MSR, &hi, &lo);
    //Clear the 11th bit of msr
    lo &= ~(APIC_ENABLE_BIT);
    //Rewrite the MSR with the new value
    cpu_wrmsr(APIC_BASE_MSR, hi, lo);
}


