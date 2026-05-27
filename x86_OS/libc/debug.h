#ifndef DEBUG_H
#define DEBUG_H
#include "types.h"
#define PANIC(msg) panic(msg, __FILE__, __LINE__) 
void panic(const char *msg, const char *file_name, uint32_t line);
#endif
