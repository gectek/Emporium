#include "stdfun.h"
#include "VGA_Text.h"
#include <stdarg.h>
char *int_to_string(int value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    //check for supported base
    if(base < 2 || base >36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;                 //rc is what we should return
    if(value < 0 && base == 10)     //prefixing '-' sign to negative numbers (only for base 10)
    {
        *ptr++ = '-';
    }
    low = ptr;                      //remembering the starting of output string
    //conversion:
    do {
        //from the string below, each digit (whether if its any base) is selected from the origin 0 [35]      
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    
    }while (value);
    //terminating the string
    *ptr-- = '\0';
    //reversing the string
    while(low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;              //swapping from first to last...and so on.
        *ptr-- = tmp;
    }
    return rc;      //rc points to the starting of the string
}
void print(const char *format, ...)
{
    va_list args;
    //creates a pointer to the list of arguments
    va_start(args, format);
    //initialize the argument list 
    while(*format != '\0')
    {
        if(*format != '%')
        {
            terminal_putchar(*format);
            format++;
            continue;
        }
        format++;   //this part of the code runs only if the *format == '%'
                    //so, incrementing the format pointer to see what's coming next(d,x,s..)

        switch(*format){
            case 'd':{
                //Decimal value
                int d = va_arg(args, int);
                
                char buffer[32];
                int_to_string(d,buffer,10);
                print_string(buffer);
                break;
            }
            case 'x':{
                int x = va_arg(args, int);
                char buffer[32];
                int_to_string(x,buffer,16);
                print_string(buffer);
                break;
            }
            case 's':{
                char *str = va_arg(args, char*);
                print_string(str);
                break;
            }
                        
        }
        format++;       //Going to the next character
    }
    va_end(args);       //End the list

}
