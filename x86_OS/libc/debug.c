#include "debug.h"
#include "stdfun.h"

void panic(const char *msg, const char *file_name, uint32_t line)
{
    __asm__("cli");
    print(msg);
    while(1)
    {
        __asm__("hlt");
    }
}
