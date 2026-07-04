#include "heap.h"
#include "pmm.h"
#include "paging.h"
#include "stdfun.h"
#include "../../libc/debug.h"
extern uintptr_t endkernel;
uintptr_t *heap_start;
uintptr_t initial_heapsize = 1024*1024;
void heap_init()
{
    //Round the heap_start to the next 4kb divisible address
    //heap_start = &endkernel;
    /*if((uintptr_t)heap_start % 4096 != 0)
    {
        heap_start = (uintptr_t*)(((uintptr_t)heap_start & 0xFFFFF000) + 4096); 
    }*/
    heap_start = (uintptr_t*)0xC0400000;//Starting heap from this address; because, between endkernel and this address there are some other non-usable memory space.
    //Allocating the memory for primary heap space
    for(uint32_t offset = 0; offset < initial_heapsize ; offset += 4096)
    {
        uintptr_t* current_virt_addr = (uintptr_t*)((uintptr_t)heap_start + offset);
        uintptr_t *physical_addr = kalloc_frame();
        map_page(physical_addr, current_virt_addr, 0x07);
    }
    //Setting up the primary header
    heap_header_t *primary_header = (heap_header_t*)heap_start;
    primary_header->magic = HEAP_MAGIC;
    primary_header->isfree = 1;
    primary_header->size = initial_heapsize - sizeof(heap_header_t);
    primary_header->next = NULL;
    primary_header->prev = NULL;
    print("Heap_init completed...\n");
}
uintptr_t *k_malloc(uintptr_t size)
{
    //ZERO CHECK(for avoiding later complications)
    if(size == 0)
    {
        return NULL;
    }
    //Rounding the size to the next 4 byte boundary
    uintptr_t aligned_size = (size + 3) & ~3;

    heap_header_t *header_tracker = (heap_header_t *)heap_start;
    //Check if whether any free memory is available or not
    while(header_tracker != NULL)
    {
        if(header_tracker->isfree && header_tracker->size >= aligned_size)
        {
            if(header_tracker->size > aligned_size + sizeof(heap_header_t) + 4)
            {
                //byte by byte analysis
                uintptr_t current = (uintptr_t)header_tracker;
                uintptr_t new_header_addr = current + aligned_size + sizeof(heap_header_t);

                heap_header_t *new_inter_header = (heap_header_t *)new_header_addr;

                //Setup the new_header
                new_inter_header->magic = HEAP_MAGIC;
                new_inter_header->isfree = 1;
                new_inter_header->size = header_tracker->size - aligned_size - sizeof(heap_header_t);
                
                //Change the links
                new_inter_header->next = header_tracker->next;
                new_inter_header->prev = header_tracker;
                if(header_tracker->next != NULL)
                {
                    header_tracker->next->prev = new_inter_header;
                }
                header_tracker->next = new_inter_header;
                header_tracker->size = aligned_size;
            }
            header_tracker->isfree = 0;
            uintptr_t allocated_addr = (uintptr_t)header_tracker + sizeof(heap_header_t);

            return (void*)allocated_addr;
        }
        header_tracker = header_tracker->next;
    }
    //Code for allocating new pages goes here(future implementations)
    return NULL;
}
void k_free(void *f_address)
{
    //The f_address will be pointing to the start of the usable address, inorder to get to the header we have to subtract sizeof(heap_header)
    heap_header_t* header_addr = (heap_header_t*)((uintptr_t)f_address - sizeof(heap_header_t));
    if(header_addr->isfree == 1 || header_addr->magic != HEAP_MAGIC)
    {
        PANIC("Kernel panic...!!!Trying to free an address which is already free");
    }
    //Setting the isfree = 1,as we are going to free this memory
    header_addr->isfree = 1;
    
    //Coalesce the right and left if there are free spaces
    if(header_addr->next != NULL && header_addr->next->isfree == 1)
    {
        //coalesce to right
        header_addr->size += header_addr->next->size + sizeof(heap_header_t);
        if(header_addr->next->next != NULL)
        {
            header_addr->next = header_addr->next->next;
            header_addr->next->prev = header_addr;
        }
        else {
            header_addr->next = NULL;
        }
    }
    if(header_addr->prev != NULL && header_addr->prev->isfree == 1)
    {
        //coalesce to left
        header_addr->prev->size += header_addr->size + sizeof(heap_header_t);
        if(header_addr->next != NULL)
        {
            header_addr->prev->next = header_addr->next;
            header_addr->next->prev = header_addr->prev;
        }
        else {
            header_addr->prev->next = NULL;
        }
    }
}









