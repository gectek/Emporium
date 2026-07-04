#include "timer.h"
#include "idt.h"
#include "pic.h"
#include "stdfun.h"
#include "../kernel/MultiTasking/scheduler.h"
#define TIME_SLICE 5
uint32_t timer_ticks = 0;    //For keeping track of how many ticks the system  has been running

void timer_phase(int hz)
{
    int divisor = 1193180/hz;       //setting the divisor
    outb(0x43, 0x36);               //setting up the command reg
    outb(0x40, divisor & 0xFF);     //Sending the divisor to channel0
    outb(0x40, divisor >> 8);
}
registers_t* timer_handler(registers_t *regs)
{
    timer_ticks++;
    registers_t* new_esp = regs;
    if(timer_ticks % TIME_SLICE == 0)
    {
        new_esp = schedule(regs);
    }
    return new_esp;
}
void timer_init()
{
  install_int_handler(32, timer_handler);
  IRQ_clear_mask(0);
}
void timer_delay(int ticks)
{
    unsigned long eticks;
    eticks = timer_ticks + ticks;            //eticks is the current time + the delay time
    while(timer_ticks < eticks)
    {
        __asm__ __volatile__ ("sti;hlt;cli");//if we didn't set int flag, the timer_ticks won't increase
                                             //also after sti, we will hlt the cpu(low power mode)
                                             //then cli for checking the while loop condition
    }
}
