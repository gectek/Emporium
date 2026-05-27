#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "types.h"
#include "stdfun.h"
#include "pic.h"
#include "idt.h"
#include "serial.h"
void keyboard_init();
void keyboard_handler(registers_t regs);
void shift_state(uint8_t ASCII);
#endif
