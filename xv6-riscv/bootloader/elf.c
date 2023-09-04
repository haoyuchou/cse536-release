#include "types.h"
#include "param.h"
#include "layout.h"
#include "riscv.h"
#include "defs.h"
#include "buf.h"
#include "elf.h"

#include <stdbool.h>

struct elfhdr* kernel_elfhdr;
struct proghdr* kernel_phdr;

uint64 find_kernel_load_addr(enum kernel ktype) {
    /* CSE 536: Get kernel load address from headers */
    
    // point to RAMDISK
    kernel_elfhdr = (struct elfhdr*)RAMDISK;
    // Get the offset, phoff in the elfhdr
    uint64 offset = kernel_elfhdr -> phoff;
    // Get the size of the program header
    ushort program_hdr_size = kernel_elfhdr -> phsize;
    // Program header section
    text_section_addr = RAMDISK + offset + program_hdr_size;
    kernel_phdr = (struct proghdr*)text_section_addr; 
    // Starting address of the text section, vaddr
    return kernel_phdr -> vaddr;
}

uint64 find_kernel_size(enum kernel ktype) {
    /* CSE 536: Get kernel binary size from headers */
    return 0;
}

uint64 find_kernel_entry_addr(enum kernel ktype) {
    /* CSE 536: Get kernel entry point from headers */
    return 0;
}