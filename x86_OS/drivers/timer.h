#ifndef TIMER_H
#define TIMER_H
#include "../cpu/idt.h"
#include "../libc/types.h"
#include "io.h"
#include "VGA_Text.h"
void timer_phase(int hz);
registers_t* timer_handler(registers_t *regs);
void timer_init();
void timer_delay(int ticks);
extern uint32_t timer_ticks;
#endif

