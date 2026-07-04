#include "serial.h"
#include "stdfun.h"
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}
void serial_configure_line(unsigned short com)
{
/* Bit:
            | 7 | 6 | 5 4 3 | 2  |  1 0 |
* Content:  | d | b | p r ty | s | d l |
* Value:    | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
*/
    outb(SERIAL_LINE_COMMAND_PORT(com),0x03);
}

void serial_fifo_buffer_config(unsigned short com)
{
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}
void serial_modem_config(unsigned short com)
{
/*
Bit:      | 7 | 6 | 5 | 4  | 3    | 2  | 1   | 0   |
Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |*/
    //0x03: rts=1, dtr=1(ready to send and recieve)
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}
int serial_is_transmit_fifo_empty(unsigned short com)
{
    //0x20 : 0010 0000(5th bit shows if it empty or not)
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}
void serial_write(unsigned short com,char *buf)
{
    //loop through the string until null character comes
    while(*buf != '\0'){
        /*Busy waiting: wait till the fifo transmit buffer becomes empty*/
        while(serial_is_transmit_fifo_empty(com) == 0);
        //once it become empty (5th bit=1) break out from the loop
        outb(SERIAL_DATA_PORT(com), *buf);
        //fb_write_cell(i,*buf,FB_BLACK, FB_WHITE);
        //i=i+2;
        buf++;//incrementing the address to the next character
    }
}
void serial_init()
{
    //serial port communication setup
    serial_configure_baud_rate(SERIAL_COM1_BASE, 1);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_fifo_buffer_config(SERIAL_COM1_BASE);
    serial_modem_config(SERIAL_COM1_BASE);
    serial_write(SERIAL_COM1_BASE,"Welcome to GEctOS...\n");
    serial_write(SERIAL_COM1_BASE,"Kernel started succesfully\n");
}
