/* This file contains code for a generic page fault handler for processes. */
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

int loadseg(pagetable_t pagetable, uint64 va, struct inode *ip, uint offset, uint sz);
int flags2perm(int flags);

/* CSE 536: (2.4) read current time. */
uint64 read_current_timestamp() {
  uint64 curticks = 0;
  acquire(&tickslock);
  curticks = ticks;
  wakeup(&ticks);
  release(&tickslock);
  return curticks;
}

bool psa_tracker[PSASIZE];

/* All blocks are free during initialization. */
void init_psa_regions(void)
{
    for (int i = 0; i < PSASIZE; i++) 
        psa_tracker[i] = false;
}

/* Evict heap page to disk when resident pages exceed limit */
void evict_page_to_disk(struct proc* p) {
    /* Find free block */
    int blockno = 0;
    /* Find victim page using FIFO. */
    /* Print statement. */
    print_evict_page(0, 0);
    /* Read memory from the user to kernel memory first. */
    
    /* Write to the disk blocks. Below is a template as to how this works. There is
     * definitely a better way but this works for now. :p */
    struct buf* b;
    b = bread(1, PSASTART+(blockno));
        // Copy page contents to b.data using memmove.
    bwrite(b);
    brelse(b);

    /* Unmap swapped out page */
    /* Update the resident heap tracker. */
}

/* Retrieve faulted page from disk. */
void retrieve_page_from_disk(struct proc* p, uint64 uvaddr) {
    /* Find where the page is located in disk */

    /* Print statement. */
    print_retrieve_page(0, 0);

    /* Create a kernel page to read memory temporarily into first. */
    
    /* Read the disk block into temp kernel page. */

    /* Copy from temp kernel page to uvaddr (use copyout) */
}


void page_fault_handler(void) 
{
    /* Current process struct */
    struct proc *p = myproc();

    /* Track whether the heap page should be brought back from disk or not. */
    bool load_from_disk = false;

    /* Find faulting address. */
     // stval register
    uint64 exact_byte_addr = r_stval();
    // base address, find by 
    // right- shifting and left-shifting the page offset-related bits
    // the offset bit is from 0 to 11
    uint64 faulting_addr = ((exact_byte_addr >> 12) << 12); 
   
    print_page_fault(p->name, faulting_addr);

    /* Check if the fault address is a heap page. Use p->heap_tracker */
    // used heap_tracker, track faulting address at which heap page
    int heap_tracker_region = -1;
    for(int page = 0; page < p->used_heap_page_tracker; page ++){
        // if faulting address is within this heap page address range
        if (p->heap_tracker[page].addr < faulting_addr && faulting_addr < p->heap_tracker[page].addr + PGSIZE){
            heap_tracker_region = page;
            break;
        }
    }
    if (heap_tracker_region != -1) {
        goto heap_handle;
    }
    // must be a page from the program binary that is not yet loaded.
    struct inode *ip = namei(p -> name);
    struct elfhdr elf;
    struct proghdr ph;
    int i, off;
    begin_op();
    ilock(ip);

  // Check ELF header
  readi(ip, 0, (uint64)&elf, 0, sizeof(elf));
  // printf("read elf header");

  /// Iterate through each program section header (using the binary’s ELF)
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    // start from offset
    readi(ip, 0, (uint64)&ph, off, sizeof(ph));
    //printf("read from offset");
    if(ph.type != ELF_PROG_LOAD){
      continue;}

    // check if base address of faulting address is within the range of 
    // the program segment's virtual address (ph.vaddr) 
    // and its virtual address plus memory size (ph.vaddr + ph.memsz)
    if(faulting_addr < ph.vaddr || faulting_addr > (ph.vaddr + ph.memsz)){
        continue;}  

    uvmalloc(p -> pagetable, faulting_addr, ph.vaddr + ph.memsz, flags2perm(ph.flags));
    loadseg(p -> pagetable, ph.vaddr, ip, ph.off, ph.filesz);
    /* If it came here, it is a page from the program binary that we must load. */
    print_load_seg(faulting_addr, ph.off, ph.memsz);  
  }
  iunlockput(ip);
  end_op();

    /* Go to out, since the remainder of this code is for the heap. */
    goto out;

heap_handle:
    /* 2.4: Check if resident pages are more than heap pages. If yes, evict. */
    if (p->resident_heap_pages == MAXRESHEAP) {
        evict_page_to_disk(p);
    }

    /* 2.3: Map a heap page into the process' address space. (Hint: check growproc) */
    // Allocate a new physical page and map it to the faulted address
    uvmalloc(p->pagetable, p->heap_tracker[heap_tracker_region].addr, p->heap_tracker[heap_tracker_region].addr + PGSIZE, PTE_W);
    
    /* 2.4: Update the last load time for the loaded heap page in p->heap_tracker. */

    /* 2.4: Heap page was swapped to disk previously. We must load it from disk. */
    if (load_from_disk) {
        retrieve_page_from_disk(p, faulting_addr);
    }

    /* Track that another heap page has been brought into memory. */
    p->resident_heap_pages++;

out:
    /* Flush stale page table entries. This is important to always do. */
    sfence_vma();
    return;
}