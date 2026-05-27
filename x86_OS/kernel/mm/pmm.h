#ifndef PMM_H
#define PMM_H
#include "multiboot.h"
#define FREE 0
#define LOCK 1
void pmm_init(multiboot_info_t *mbi);
void *kalloc_frame();
void kfree_frame(void *physical_addr);
void set_bit(uint32_t index, uint32_t bit);
void clear_bit(uint32_t index, uint32_t bit);
void bitmap_free(uintptr_t base, uint64_t length);
void bitmap_lock(uintptr_t base, uint64_t length);
#endif
