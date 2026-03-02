#include "VGA_Text.h"
#include "shell.h"
#include "stdfun.h"
// VGA_TEXT_MODE_BUFFER address
uint16_t const *vgab = (uint16_t *)0x000B8000;
// function for writing to VGA_TEXT_MODE_BUFFER

terminal_t *main_term;

static inline uint16_t vga_entry(uint8_t ch, uint8_t color) {
  return (uint16_t)ch | (uint16_t)color << 8;
}
void vga_move_cursor(unsigned short pos) {
  outb(VGA_COMMAND_PORT, HIGH_BYTE_COMMAND);
  outb(VGA_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(VGA_COMMAND_PORT, LOW_BYTE_COMMAND);
  outb(VGA_DATA_PORT, pos & 0x00FF);
}

int print_string(terminal_t *term, const char *string) {
  while (*string != 0) {
    terminal_putchar(term, *string);
    string++;
  }
  return 0;
}
void terminal_clear() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      const uint16_t index = i * VGA_WIDTH + j;
      main_term->buffer[index] = vga_entry(' ', main_term->color);
    }
  }
  main_term->row = 0;
  main_term->column = 0;
  vga_move_cursor(0);
}
void terminal_init(terminal_t *term) {
  // Setting the color
  term->color = VGA_WHITE | VGA_BLACK << 4;
  // Making the buffer to point to vga buffer address
  term->buffer = (uint16_t *)vgab;
  // clearing the terminal and setting the cursor
  terminal_clear();
  shell_init();
}
void tty_putc(const char c) { terminal_putchar(main_term, c); }
void terminal_putchar(terminal_t *term, const char ch) {
  if (ch == '\n') {
    if(term->row < VGA_HEIGHT-1)
    {   
        term->column = 0;
        term->row++;
        return;
    }
    else {
        terminal_scroll(term);
        return;
    }
    }
    
  // Handle corner cases
    if (term->column == VGA_WIDTH && term->row < VGA_HEIGHT-1) {
    term->column = 0;
    term->row++;
    }
    else if (term->column == VGA_WIDTH && term->row == VGA_HEIGHT-1) {
    terminal_scroll(term);
    }

  // Step1: Calculate the position to print
  
    const uint32_t index = term->row * VGA_WIDTH + term->column;

  // Step2: Saving the character into buffer or memory
 
    term->buffer[index] = vga_entry(ch, term->color);

  // Step3: Updating the cursor
  
    vga_move_cursor(index + 1);
    term->column++;
}
void clear_char() {
    
  // First move the position variables
  if (main_term->row == 0 && main_term->column == 0)
    return;
  if (main_term->row > 0 && main_term->column == 0) {
    main_term->row--;
    main_term->column = VGA_WIDTH - 1;
  } else {
    main_term->column--;
  }
  const uint16_t index = main_term->row * VGA_WIDTH + main_term->column;
  vga_move_cursor(index);
  main_term->buffer[index] = vga_entry(' ', main_term->color);
}
void enter() {
  main_term->row++;
  main_term->column = 0;
  const uint16_t index = main_term->row * VGA_WIDTH + main_term->column;
  vga_move_cursor(index);
}
void terminal_scroll(terminal_t *term) {
    int i=0;
    //1) Move the rows up by one.

    mem_mov(term->buffer + VGA_WIDTH, term->buffer, (VGA_HEIGHT-1)*VGA_WIDTH*sizeof(uint16_t));
    
    //2) Fill the last row with
    
    for(i=(VGA_HEIGHT-1)*VGA_WIDTH ; i<VGA_HEIGHT*VGA_WIDTH; i++)
    {
        term->buffer[i] = vga_entry(' ', term->color);
    }
    
    //3) Reset the counter
    
    term->row = VGA_HEIGHT - 1;
    term->column = 0;
}
