#include "gdt.h"
#include "stdfun.h"
struct gdt_entry gdt_descriptors[4];        //array for gdt entries
struct gdt_ptr_struct gdt_ptr;                   //pointer for address and size of gdt

extern void gdt_flush(uint32_t);            //external assembly function(in gdt.s)
extern void tss_flush();
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
    gdt_ptr.limit = (sizeof(struct gdt_entry)*4) - 1;
    gdt_ptr.base = (uintptr_t)&gdt_descriptors;
    //2) setup the entries:entry 0(Null descriptor)
    gdt_set_gate(0,0,0,0,0);
    //3) entry 1(code segment)
    //Base = 0, limit = 4GiB, Access = 0x9A, gran = 0xCF
    gdt_set_gate(1,0,0xFFFFFFFF, 0x9A, 0xCF);
    //4) entry 2(data segment)
    //Base=0, limit = 4 GiB, Access = 0x92, gran = 0xCF
    gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF);
    
    tss_init();
    //load the new gdt using the assembly code
    gdt_flush((uintptr_t)&gdt_ptr);
    tss_flush();
}

static struct tss_entry tss;

void tss_init() {
    uint32_t base  = (uint32_t)&tss;
    uint32_t limit = sizeof(tss) - 1;

    // Entry 3 in GDT (0x18) — type 0x89 = present, 32-bit TSS
    gdt_set_gate(3, base, limit, 0x89, 0x00);
    gdt_descriptors[3].access &= ~0x02;
    tss.ss0  = 0x10;              // kernel data segment
    tss.esp0 = 0;                 // will be set properly if you do usermode later
    tss.iomap_base = sizeof(tss); // no I/O permission bitmap
}
void tss_set_esp0(uintptr_t esp0)
{
    tss.esp0 = esp0;
}
