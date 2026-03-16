#ifndef MULTI_BOOT_H
#define MULTI_BOOT_H
#include "types.h"
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2      
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
//I am only considering the most needed elements for PMM
typedef struct{
    uint32_t flags;
    uint32_t padded[10];    //padding (boot device info, cmdline strings, ELF symbols,..)
    uint32_t mmap_length;   //offset 44
    uint32_t mmap_addr;     //offset 48
}multiboot_info_t;
typedef struct{
    uint32_t size;          //How many bytes this specific folder takes up(software: For pointer math)
    uint64_t base_addr;     //Base address of the folder
    uint64_t length;        //How many bytes this RAM chunk is
    uint32_t type;          //Danger flag(1 = Safe ram, anything else = reserved)
}__attribute__((packed)) multiboot_mmap_entry_t;
#endif
