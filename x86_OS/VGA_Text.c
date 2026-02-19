#include "VGA_Text.h"
const uint8_t VGA_WIDTH = 80;
const uint8_t VGA_HEIGHT = 25;
//VGA_TEXT_MODE_BUFFER address
uint16_t const *vgab = (uint16_t*)0x000B8000;
//function for writing to VGA_TEXT_MODE_BUFFER
uint32_t terminal_row;
uint32_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

static inline uint16_t vga_entry(uint8_t ch, uint8_t color)
{
    return (uint16_t) ch | (uint16_t) color << 8;
}
void vga_move_cursor(unsigned short pos){
    outb(VGA_COMMAND_PORT, HIGH_BYTE_COMMAND);
    outb(VGA_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(VGA_COMMAND_PORT, LOW_BYTE_COMMAND);
    outb(VGA_DATA_PORT, pos & 0x00FF);    
}
int print_string(const char *string)
{
    while(*string != 0)
    {
        terminal_putchar(*string);  
        string++;
        
    }
    return 0;
}
void terminal_init(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = VGA_WHITE | VGA_BLACK << 4;
    terminal_buffer = (uint16_t*)vgab;
    vga_move_cursor(0);
}
void terminal_putchar(const char ch)
{
   if(ch == '\n')
    {
        terminal_column = 0;
        terminal_row++;
        return;
    }
    
    //Step1: Calculate the position to print
    const uint32_t index = terminal_row * VGA_WIDTH + terminal_column;
    //Step2: Saving the character into buffer or memory 
    terminal_buffer[index] = vga_entry(ch, terminal_color);
    //Step3: Updating the cursor
    vga_move_cursor(index+1);
    terminal_column++;
    //Handle corner cases
    if(terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        terminal_row++;
    }

}
void clear_char()
{
    //First move the global variables
    if(terminal_row == 0 && terminal_column == 0)
        return;
    if(terminal_row>0 && terminal_column == 0){
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    }
    else {
        terminal_column--;
    }
    const uint32_t index = terminal_row * VGA_WIDTH + terminal_column;
    vga_move_cursor(index);
    terminal_buffer[index] = vga_entry(' ', terminal_color);

}
void enter()
{
    terminal_row++;
    terminal_column=0;
    const uint32_t index = terminal_row * VGA_WIDTH + terminal_column;
    vga_move_cursor(index);
}
