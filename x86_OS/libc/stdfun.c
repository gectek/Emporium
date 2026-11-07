#include "stdfun.h"
#include <stdarg.h>
#include "serial.h"
#include "debug.h"
char *int_to_string(uint32_t value, char *str, int base)
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
        //from the string below, each digit (whether if its any base) is selected from the origin as 0 [35]      
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
int ASCII_to_int(const char *str, int base)
{
    int result = 0;
    if(base == 10)
    {
        for(int i = 0; i < str_len(str); i++)
        {
            result *= 10;
            result += str[i] - '0';
        }
    }
    else if(base == 16)
    {
        int i=0;
        if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        {
            i = 2;
        }
        for(;i<str_len(str);i++)
        {
            result = result << 4;
            if(str[i] <= '9' && str[i] >= '0')
            {
                result += str[i] - '0';
            }
            else if(str[i] > '9')
            {
                result += (str[i] - 'a') + 10;
            }
        }

    }
    return result;
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
            terminal_putchar(main_term, *format);
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
                print_string(main_term, buffer);
                break;
            }
            case 'x':{
                int x = va_arg(args, int);
                char buffer[32];
                int_to_string(x,buffer,16);
                print_string(main_term, buffer);
                break;
            }
            case 's':{
                char *str = va_arg(args, char*);
                print_string(main_term, str);
                break;
            }
        }
        format++;       //Going to the next character
    }
    va_end(args);       //End the list
}
int str_len(const char *string)
{
    int count=0;
    while(*string != '\0')
    {
        count++;
        string++;
    }
    return count;
}
int str_cmp(const char *s1,const char *s2)
{
    const uint32_t count = str_len(s1);
    uint32_t i=0;
    if(str_len(s1) != str_len(s2))
        return -1;
    else{
        while(i<count)
        {
            if(s1[i] == s2[i])
            {
                i++;
                continue;
            }
            else {
                return -1;
            }
        }
    }
    return 0;
}
void mem_mov(void *src, void *dest, int size)
{
    uint8_t *s = (uint8_t *)src;
    uint8_t *d = (uint8_t *)dest;
    int i;
    if(src == dest)
    {
        return;
    }
    else if(src < dest)
    {
        for(i=size;i>0;i--)
        {
            d[i-1] = s[i-1];
        }
    }
    else 
    {
        for(i=0;i<size;i++)
        {
            d[i] = s[i];
        }
    }
}
void mem_set(void *start_addr, char data, uint32_t len)
{
    uintptr_t *start = (uintptr_t*)start_addr;
    for(int i=0; i< len; i++)
    {
        *(start+i) = data;
    }
}






