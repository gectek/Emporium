#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "types.h"
#include "stdfun.h"
#include "pic.h"
#include "idt.h"
#include "serial.h"
#define KB_BUFFER_SIZE 256
void keyboard_init();
registers_t* keyboard_handler(registers_t *regs);
void shift_state(uint8_t ASCII);
#endif
