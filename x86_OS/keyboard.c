#include "keyboard.h"
#include "VGA_Text.h"
#include "idt.h"
#include "pic.h"
#include "serial.h"
#include "types.h"
char keymaps[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 0x00 - 0x09 */
    '9', '0', '-', '=', '\b', /* 0x0A - 0x0E */
    '\t', /* 0x0F */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* 0x10 - 0x1C */
    0, /* 0x1D: LCtrl */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', /* 0x1E - 0x29 */
    0, /* 0x2A: LShift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', /* 0x2B - 0x35 */
    0, /* 0x36: RShift */
    '*',
    0, /* 0x38: LAlt */
    ' ',0, /* 0x39: Space - 0x3A: Caps*/
    0,	/* 59 - F1 key ... > */  
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
char shift_keymaps[128] = {
    0,0,'!','@','#','$','%','^','&','*','(',')','_','+','\b',    //0x00 - 0x0E
    0,'Q','W','E','R','T','Y','U','I','O','P','{','}','\n', //enter = 0x1C
    0, // LCtrl = 0x1D
    'A','S','D','F','G','H','J','K','L',':','"','~',        //0x1E - 0x29
    0, //0x2A - Lshift(Further research)
    '|','Z','X','C','V','B','N','M','<','>','?',    //0x2B - 0x35
    0, //0x36 - Rshift(Further research)

};
bool shift_pressed,ctrl_pressed;

void keyboard_init()
{
    //Installing the keyboard handler to the irq_routines table
    install_irq_handler(1, &keyboard_handler); 
    
    //Clearing the irq_mask 1
    IRQ_clear_mask(1);  
    serial_write(SERIAL_COM1_BASE, "Keyboard Initialized...");
}
void keyboard_handler(registers_t regs)
{
    uint8_t scancode;
    unsigned char ascii;
    //read from the keyboards data buffer
    scancode = inb(0x60);
    //Checking the shift status
    if(scancode == 0x2A || scancode == 0x36)
    {    
        shift_pressed = true;
        return;             //For not affecting the Printing
    }
    if(scancode & 0x80)     //Checking if the MSB is set or not(For checking if any keys are released)
    {
        uint16_t released_key = scancode & 0x7F;    //To find the pressing scancode
        if(released_key == 0x2A || released_key == 0x36)
        {
            shift_pressed = false;                  //Disabling shift
        }
    }
    else if (shift_pressed == true) {

        uint8_t ASCII = shift_keymaps[scancode];//From the shift_array
        key_print(ASCII);
    }
    else{
        ascii = keymaps[scancode];      //Normal state
        key_print(ascii);
    }
}
void key_print(uint8_t ascii)
{
    if(ascii == '\b')   //For  backspace
    {
        clear_char(); 
    }
    else if(ascii == '\n')  //For Enter
    {
        enter();
    }
    else {
        terminal_putchar(ascii);    //Normal character
    } 
}


