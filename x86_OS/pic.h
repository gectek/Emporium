#ifndef PIC_H
#define PIC_H
#include "types.h"

#define PIC1            0x20    /*IO base address for master PIC*/
#define PIC2            0xA0    /*IO base address for slave PIC*/
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define PIC_EOI         0x20    //End-Of-Interrupt command code

#define ICW1_ICW4       0x01    /*Indicates ICW4 will be present(wait till 4 configs)*/
#define ICW1_SINGLE     0X02    /*Single mode (cascade mode)*/
#define ICW1_INTERVAL4  0x04    /*Call address interval = 4(8)*/
#define ICW1_LEVEL      0X08    /*Level triggered mode (edge triggered)*/
#define ICW1_INIT       0X10    /*Initialization -- required*/

#define ICW4_8086       0x01    //8086 mode(8080)
#define ICW4_AUTO       0x02    //Auto EOI(normal)
#define ICW4_BUF_SLAVE  0x08    //Buffered mode -- slave
#define ICW4_BUF_MASTER 0x0C    //Buffered mode -- master
#define ICW4_SFNM       0x10    //Special Fully nested (not)

#define IRQ_CASCADE 2
#define PIC_READ_ISR 0x0b
#define PIC_READ_IRR 0x0a

void PIC_sendEOI(uint8_t irq);
void PIC_remap(uint32_t offset1, uint32_t offset2);
void PIC_disable(void);
void IRQ_set_mask(uint8_t IRQLine);
void IRQ_clear_mask(uint8_t IRQLine);
/*static uint16_t __pic_get_irq_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
*/
#endif
