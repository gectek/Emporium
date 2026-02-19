#ifndef TIMER_H
#define TIMER_H
#include "idt.h"
#include "types.h"
#include "idt.h"
#include "io.h"
#include "VGA_Text.h"
void timer_phase(int hz);
void timer_handler(registers_t regs);
void timer_install();
void timer_delay(int ticks);
#endif

