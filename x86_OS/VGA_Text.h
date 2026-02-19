#ifndef VGA_H
#define VGA_H
#include "io.h"
#include "types.h"
#define VGA_WHITE        15
#define VGA_BLACK        0
//cursor i/o ports
#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT    0x3D5
//cursor higher/lower nibbles
#define HIGH_BYTE_COMMAND 14
#define LOW_BYTE_COMMAND  15
void vga_move_cursor(unsigned short pos);
int print_string(const char *string);
void terminal_init(void);
void terminal_putchar(const char ch);
void clear_char();
void enter();
#endif
