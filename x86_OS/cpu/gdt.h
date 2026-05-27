#include "types.h"
#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H
struct gdt_entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));

struct gdt_ptr_struct{
    uint16_t limit;     //size of the GDT table -1
    uint32_t base;      //address of the first GDT entry
}__attribute__((packed));
void gdt_init();
void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
#endif
