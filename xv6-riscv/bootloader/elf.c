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
    ushort program_hdr_size = kernel_elfhdr -> ehsize;
    // Program header section
    uint64 text_section_addr = RAMDISK + offset + program_hdr_size;
    kernel_phdr = (struct proghdr*)text_section_addr; 
    // Starting address of the text section, vaddr
    return kernel_phdr -> vaddr;
}

uint64 find_kernel_size(enum kernel ktype) {
    /* CSE 536: Get kernel binary size from headers */
    // kernel1 is 278,088 bytes
    // obtain the size using ELF header

    // point to RAMDISK, where the kernel is currently loaded
    kernel_elfhdr = (struct elfhdr*)RAMDISK;
     // https://stackoverflow.com/questions/2995347/how-can-i-find-the-size-of-a-elf-file-image-with-header-information
    // e_shoff + ( e_shentsize * e_shnum )
    uint64 offset = kernel_elfhdr -> shoff;
    ushort shentsize = kernel_elfhdr -> shentsize;
    ushort shnum = kernel_elfhdr -> shnum;

    return offset + (shentsize * shnum);
}

uint64 find_kernel_entry_addr(enum kernel ktype) {
    /* CSE 536: Get kernel entry point from headers */
    kernel_elfhdr = (struct elfhdr*)RAMDISK;
    return kernel_elfhdr -> entry;
}