#include "gdt.h"
struct gdt_entry gdt_descriptors[3];        //array for gdt entries
struct gdt_ptr_struct gdt_ptr;                   //pointer for address and size of gdt

extern void gdt_flush(uint32_t);            //external assembly function(in gdt.s)

//function to fill one entry
void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    //1) set the base address
    gdt_descriptors[num].base_low = (base & 0xFFFF);
    gdt_descriptors[num].base_middle = ((base>>16) & 0xFF);
    gdt_descriptors[num].base_high = ((base>>24)& 0xFF);
    //2) set the limit
    gdt_descriptors[num].limit_low = limit & 0xFFFF;//the high part is with granularity
    //3) set the top 4 bits of limit and flags
    gdt_descriptors[num].granularity = ((limit>>16)&0xF);
    gdt_descriptors[num].granularity |= gran & 0xF0;

    gdt_descriptors[num].access = access;

}
void gdt_init(){
    //1) setup the gdt pointers
    gdt_ptr.limit = (sizeof(struct gdt_entry)*3) - 1;
    gdt_ptr.base = (uint32_t)&gdt_descriptors;
    //2) setup the entries:entry 0(Null descriptor)
    gdt_set_gate(0,0,0,0,0);
    //3) entry 1(code segment)
    //Base = 0, limit = 4GiB, Access = 0x9A, gran = 0xCF
    gdt_set_gate(1,0,0xFFFFFFFF, 0x9A, 0xCF);
    //4) entry 2(data segment)
    //Base=0, limit = 4 GiB, Access = 0x92, gran = 0xCF
    gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF);
    
    //load the new gdt using the assembly code
    gdt_flush((uint32_t)&gdt_ptr);
}

