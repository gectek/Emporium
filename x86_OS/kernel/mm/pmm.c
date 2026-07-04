#include "pmm.h"
#include "debug.h"
#include "multiboot.h"
#include "stdfun.h" 
extern uint32_t endkernel;
uint32_t *memory_bitmap;
uint32_t total_blocks=0;

void pmm_init(multiboot_info_t *mbi)
{
    uint32_t highest_addr=0;
    uint32_t current_chunk_end;
    if(!(mbi->flags >> 6 & 0x1))
    {
        PANIC("MEMORY MAP NOT GIVEN BY BOOTLOADER...!!");
    }
    uintptr_t phys_kernel_end = (uintptr_t)&endkernel - 0xC0000000;
    memory_bitmap = &endkernel;
    multiboot_mmap_entry_t *folder =(multiboot_mmap_entry_t*)(mbi->mmap_addr+0xC0000000);
    uintptr_t folder_end = mbi->mmap_addr + 0xC0000000 + mbi->mmap_length; 
    
    highest_addr = 0xFFFFFFFF;
    total_blocks = (highest_addr)/0x1000 + 1;
    print("mmap_addr phys = %x\n", mbi->mmap_addr);
print("mmap_addr virt = %x\n", mbi->mmap_addr + 0xC0000000);
    //2) Filling whole bitmap as USED
    
    for(uint32_t i =0; i < (total_blocks + 31)/32 ; i++)
    {
        memory_bitmap[i] = 0xFFFFFFFF;
    }

    folder = (multiboot_mmap_entry_t*)(mbi->mmap_addr+0xC0000000); //resetting the folder to starting address of folders
    
    //3) Now, we have to set all memory of type 1 to FREE
    while((uintptr_t)folder < folder_end)
    {
        if(folder->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            bitmap_free(folder->base_addr, folder->length);
        }
        folder = (multiboot_mmap_entry_t*)((uintptr_t)folder + folder->size + 4);
    }

    //4) Next The kernel and bitmap addresses have to be marked as USED(Infact from 0 to 0x100000 and bitmap)
    uint32_t bitmap_length_bytes = (total_blocks+7)/8;
    uintptr_t kernel_start = 0x100000;
    uintptr_t bitmap_start = phys_kernel_end;
    uintptr_t bitmap_end = bitmap_start + bitmap_length_bytes;

    //Lock the whole lower region
    bitmap_lock(0x0, kernel_start);
    uint32_t kernel_bitmap_len = bitmap_end - kernel_start;
    
    //Lock the kernel and bitmap
    bitmap_lock(kernel_start, kernel_bitmap_len);
    
    //for(int i =0; i<1000; i++)
    //    print("%x",memory_bitmap[i]);

    
    /*folder = (multiboot_mmap_entry_t*)mbi->mmap_addr;
    while((uintptr_t)folder < folder_end)
    {
        print("\nStart: %x | Length: %x | Size: %d | Type: %d\n",(uint32_t)folder->base_addr,(uint32_t)folder->length,folder->size,folder->type);
    folder = (multiboot_mmap_entry_t*)((uintptr_t)folder + folder->size + 4);
    }*/
    print("PMM Init Successfully Completed...\n");
}
void *kalloc_frame()
{
    uint32_t i =0,Bit=0,block_number;
    uint32_t start_address = 0x0;
    while(memory_bitmap[i] == 0xFFFFFFFF)
    {
        if(i > (total_blocks+31)/32)
        {
            print("No Available memory\n%d\n",i);
            return NULL;
        }
        i++;
    }
    for(;Bit<32;Bit++)
    {
        if(!(memory_bitmap[i] & (1<<Bit)))
            break;
            //found the FREE bit
    }
    set_bit(i,Bit);     //Setting the bit corresponding to the address
    //Remember each bit is an address
    block_number = (i*32)+Bit;
    uintptr_t ret_addr = start_address+(block_number*0x1000);
    return (void*)ret_addr;
}
void kfree_frame(void *physical_addr)
{
    uintptr_t addr = (uintptr_t)physical_addr;  //For doing calculations on this address.
    //Ignore NULL pointers
    if(addr == 0)
        return;
    //Find the index of bitmap
    uint32_t frame_number = (addr+4095) / 4096;
    
    //Out of bounds condition
    if(frame_number >= total_blocks)
        return;
    
    //Find the index and bit of the given address
    uint32_t index = frame_number/32;
    uint32_t bit = frame_number%32;
    
    if(!(memory_bitmap[index] & (1<<bit)))
    {
        print("Memory is already free'ed\n");
        return;
    }
    //Change it to FREE
    clear_bit(index, bit);
    print("Memory free'ed in address 0x%x", addr);
}

void set_bit(uint32_t index,uint32_t bit)
{
    memory_bitmap[index] |= (1<<bit);
}
void clear_bit(uint32_t index, uint32_t bit)
{
    memory_bitmap[index] &= ~(1<<bit);
}
void bitmap_free(uintptr_t base, uint64_t length)
{
    
    uint32_t index,Bit;
    //Calculate the starting frame
    uint32_t start_frame = (base+4095) / 4096;
    //Calculate total frames
    uint32_t total_frames_to_free = (length+4095) / 4096; 
    //Now we have to FREE these frames
    uint32_t current_frame = start_frame;
    while(current_frame < start_frame + total_frames_to_free)
    {
        index = current_frame/32;
        Bit = current_frame % 32;
        clear_bit(index, Bit);
        current_frame++;
    }

}

void bitmap_lock(uintptr_t base, uint64_t length)
{
    
    uint32_t index,Bit;
    //Calculate the starting frame
    uint32_t start_frame = (base+4095) / 4096;
    //Calculate total frames
    uint32_t total_frames_to_lock = (length+4095) / 4096; 
    //Now we have to FREE these frames
    uint32_t current_frame = start_frame;
    while(current_frame < start_frame + total_frames_to_lock)
    {
        index = current_frame/32;
        Bit = current_frame % 32;
        set_bit(index, Bit);
        current_frame++;
    }
}



