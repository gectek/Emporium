#ifndef VGA_H
#define VGA_H
#include "io.h"
#include "types.h"
#define VGA_WHITE 15
#define VGA_BLACK 0
#define VGA_RED 4
#define VGA_BLUE 1
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
//cursor i/o ports
#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT    0x3D5
//cursor higher/lower nibbles
#define HIGH_BYTE_COMMAND 14
#define LOW_BYTE_COMMAND  15

typedef struct{
    uint32_t row;
    uint32_t column;
    uint8_t color;
    uint16_t *buffer;
}terminal_t;
extern terminal_t *main_term;
void vga_move_cursor(unsigned short pos);
int print_string(terminal_t *term, const char *string);
void terminal_init(terminal_t *term);
void terminal_clear();  //driver function for cli
void terminal_scroll(terminal_t *term);
void terminal_putchar(terminal_t *term, const char c);
void tty_putc(const char c);
void clear_char();
void enter();
#endif
