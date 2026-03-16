#ifndef STDFUN_H
#define STDFUN_H
#include "VGA_Text.h"
char *int_to_ASCII(uint32_t value, char *str,int base);
int ASCII_to_int(const char *str,int base);
void print(const char *format, ...);
int str_len(const char *c);
int str_cmp(const char *s1, const char *s2);
void mem_mov(void *src, void *dest, int size);
#endif
