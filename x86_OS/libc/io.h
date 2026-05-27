#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H
#include "types.h"
static inline void outb(uint16_t port, uint8_t val);
static inline unsigned char inb(uint16_t port);
static inline void io_wait(void);
static inline void outb(uint16_t port, uint8_t val){
    __asm__ volatile ("outb %b0, %w1" : :"a"(val), "Nd"(port) : "memory");
}
static inline uint8_t inb(uint16_t port){
    uint8_t ret;
    __asm__ volatile ("inb %w1, %b0"
                    : "=a" (ret)
                    : "Nd" (port)
                    : "memory");
    return ret;
}
static inline void io_wait(){
    outb(0x80,0);
}
#endif
