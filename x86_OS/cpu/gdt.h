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

// TSS structure (minimal)
struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));
void gdt_init();
void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void tss_init();
void tss_set_esp0(uintptr_t esp0);
#endif
