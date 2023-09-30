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
int track_continu_psa_block(int block);
int find_victum_page(struct proc* p);

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
    /* Find free block, PSA area */
    int blockno = track_continu_psa_block(4);
    /* Find victim page using FIFO. */
    int victum_page_idx = find_victum_page(p);
    // startblock let us know this page was swapped
    p->heap_tracker[victum_page_idx].startblock = blockno;
    p->heap_tracker[victum_page_idx].last_load_time = 0xFFFFFFFFFFFFFFFF;
    /* Print statement. */
    print_evict_page(p->heap_tracker[victum_page_idx].addr, blockno);
    /* Read memory from the user to kernel memory first. */
    // allocate a kernel page !!!!!!!
    char* kernel_page = kalloc();
    copyin(p->pagetable, kernel_page, p->heap_tracker[victum_page_idx].addr, PGSIZE);
    /* Write to the disk blocks. Below is a template as to how this works. There is
     * definitely a better way but this works for now. :p */
    // 4 Block to store a 4 KB page
    struct buf* b;
    for (int start_block = blockno, i = 0; i<PGSIZE; start_block++, i += BSIZE){
        b = bread(1, PSASTART+(start_block));
        // Copy page contents to b.data using memmove.
        memmove(b->data, kernel_page + i, BSIZE);
        // track this psa block as used
        psa_tracker[start_block] = true;
        bwrite(b);
        brelse(b);
    }

    /* Unmap swapped out page */
    uvmunmap(p->pagetable, p->heap_tracker[victum_page_idx].addr, 1, 0);
    /* Update the resident heap tracker. */
    p->resident_heap_pages --;
}

int track_continu_psa_block(int block){
    // return the start PSA block
    uint64 remain_block = block;
    for(int i = 0; i < PSASIZE; i++){
        // if this PSA block is already used
        printf("block number: %d\n", i);
        printf("psa is used: %d\n", psa_tracker[i]);
        if (psa_tracker[i]){
            remain_block = block;
        }else{
            remain_block -= 1;
            if(remain_block == 0){
                return i - block + 1;
            }
        }
    }
    return -1;
}

int find_victum_page(struct proc* p){
    uint64 fifo = p->heap_tracker[0].last_load_time;
    int idx = 0;
    for(int i = 0; i < MAXHEAP; i++){
        if (p->heap_tracker[i].last_load_time < fifo){
            idx = i;
            fifo = p->heap_tracker[i].last_load_time;
        }
    }
    return idx;
}

/* Retrieve faulted page from disk. */
void retrieve_page_from_disk(struct proc* p, int heap_tracker_region) {
    /* Find where the page is located in disk */
    int startblock = p->heap_tracker[heap_tracker_region].startblock;
    // the user page address that we are gonna copy yo
    uint64 va = p->heap_tracker[heap_tracker_region].addr;
    // according to the startblock, load from PSA
    struct buf *b;
    for (int i = startblock; i<4; i++, va += BSIZE){
        psa_tracker[i] = false;
        b = bread(1, PSASTART+i);
        copyout(p->pagetable, va, (char*)b->data, BSIZE);
        brelse(b);
    }

    p->heap_tracker[heap_tracker_region].last_load_time = read_current_timestamp();
    p->heap_tracker[heap_tracker_region].startblock = -1;
    /* Print statement. */
    print_retrieve_page(va, startblock);

    /* Create a kernel page to read memory temporarily into first. */
    
    /* Read the disk block into temp kernel page. */

    /* Copy from temp kernel page to uvaddr (use copyout) */
}


void page_fault_handler(void) 
{
    /* Current process struct */
    struct proc *p = myproc();

    /* Track whether the heap page should be brought back from disk or not. */
    //bool load_from_disk = false;

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
    for(int page = 0; page < MAXHEAP; page ++){
        // if faulting address is within this heap page address range
        if (p->heap_tracker[page].addr <= faulting_addr && faulting_addr < p->heap_tracker[page].addr + PGSIZE){
            heap_tracker_region = page;
            break;
        }
    }
    if (heap_tracker_region != -1) {
        //printf("go to heap yoooooo");
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

    uvmalloc(p -> pagetable, ph.vaddr, ph.vaddr + ph.memsz, flags2perm(ph.flags));
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
    p->heap_tracker[heap_tracker_region].last_load_time = read_current_timestamp();
    p->heap_tracker[heap_tracker_region].loaded = true;
    p->heap_tracker[heap_tracker_region].startblock = -1;
    /* 2.4: Track total in-memory (or resident) heap pages*/
    p->resident_heap_pages++;
    /* 2.4: Heap page was swapped to disk previously. We must load it from disk. */
    if (p->heap_tracker[heap_tracker_region].startblock != -1) {
        retrieve_page_from_disk(p, heap_tracker_region);
    }

    /* Track that another heap page has been brought into memory. */
    //p->resident_heap_pages++;

out:
    /* Flush stale page table entries. This is important to always do. */
    sfence_vma();
    return;
}