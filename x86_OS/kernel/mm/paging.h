#ifndef PAGING_H
#define PAGING_H
#include "idt.h"
#define PAGE_DIR_VIRTUAL 0xFFFFF000
void map_page(void *phys_addr, void *virtual_addr, unsigned int flags);
void tlb_flush(void *virtual_addr);
void pagefault_handler(registers_t *regs);
void pagefault_init();
#endif
