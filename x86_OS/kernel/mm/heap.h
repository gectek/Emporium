#ifndef HEAP_H
#define HEAP_H
#define HEAP_MAGIC 0xDEADBEEF
#include "../../libc/types.h"
typedef struct heap_header{
    uint32_t magic;
    uint32_t size;
    struct heap_header* prev;
    struct heap_header* next;
    uint8_t isfree;
}heap_header_t;
void heap_init();
uintptr_t *k_malloc(uintptr_t size);
void k_free(void *f_address);
#endif
