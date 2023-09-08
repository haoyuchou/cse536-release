/* These files have been taken from the open-source xv6 Operating System codebase (MIT License).  */

#include "types.h"
#include "param.h"
#include "layout.h"
#include "riscv.h"
#include "defs.h"
#include "buf.h"
#include "measurements.h"
#include <stdbool.h>

void main();
void timerinit();

/* entry.S needs one stack per CPU */
__attribute__ ((aligned (16))) char bl_stack[STSIZE * NCPU];

/* Context (SHA-256) for secure boot */
SHA256_CTX sha256_ctx;

/* Structure to collects system information */
struct sys_info {
  /* Bootloader binary addresses */
  uint64 bl_start;
  uint64 bl_end;
  /* Accessible DRAM addresses (excluding bootloader) */
  uint64 dr_start;
  uint64 dr_end;
  /* Kernel SHA-256 hashes */
  BYTE expected_kernel_measurement[32];
  BYTE observed_kernel_measurement[32];
};
struct sys_info* sys_info_ptr;

extern void _entry(void);
void panic(char *s)
{
  for(;;)
    ;
}

/* CSE 536: Boot into the RECOVERY kernel instead of NORMAL kernel
 * when hash verification fails. */
void setup_recovery_kernel(void) {
}

/* CSE 536: Function verifies if NORMAL kernel is expected or tampered. */
bool is_secure_boot(void) {
  //bool verification = true;

  /* Read the binary and update the observed measurement 
   * (simplified template provided below) */
   // initialize hash_obs
  sha256_init(&sha256_ctx);
  //struct buf b;
  // Update hashobs using sha256_update. Pass as argument the entire kernel binary.
  // kernel is loaded at RAMDISK
  uint kernelSize = find_kernel_size(NORMAL);
  sha256_update(&sha256_ctx, (uchar*)RAMDISK, kernelSize);
  sha256_final(&sha256_ctx, sys_info_ptr->observed_kernel_measurement);

  /* Three more tasks required below: 
   *  1. Compare observed measurement with expected hash
   *  2. Setup the recovery kernel if comparison fails
   *  3. Copy expected kernel hash to the system information table */
  for(int i = 0; i<32; i++){
    // trusted_kernel_hash in bootloader/measurements.h.
    sys_info_ptr ->expected_kernel_measurement[i] = trusted_kernel_hash[i];
  }
  /*if (!verification)
    setup_recovery_kernel();*/
  // compare expected_kernel_measurement to observed_kernel_measurement
  for (int i = 0; i<32; i++){
    if (sys_info_ptr -> observed_kernel_measurement[i] != sys_info_ptr -> expected_kernel_measurement){
      return false;
    }
  }
  
  return true;
}

void copyKernelTo(uint64 copySize, uint64 copyToAddress, enum kerenl ktype)
{
  // use the kernel_copy function to copy from buffer to kerenl
  int blockno = 4;
  // exclude atleast the kernel ELF header (first 4KB) 
  //int offset = 4;
  //blockno = blockno + offset;
  int sizeCopied = 0;
  struct buf buffer;
  while (sizeCopied < copySize){
    // exclude first 4 kb
    buffer.blockno = blockno;
    // void kernel_copy(enum kernel ktype, struct buf *b)
    // kernel_copy copy BSIZE every time
    kernel_copy(ktype, &buffer);
    // copy from buffer to copyToAddress
    uint64 target_address = copyToAddress + ((blockno - 4) * BSIZE);
    memmove((char*)target_address, buffer.data, BSIZE);
    // update buffer number
    blockno ++;
    sizeCopied = sizeCopied + BSIZE;
  }
}

void setupKernel(enum kernel ktype){
/* CSE 536: Load the NORMAL kernel binary (assuming secure boot passed). */
  // this function return the address of kernload_start
  uint64 kernel_load_addr       = find_kernel_load_addr(ktype);
  uint64 kernel_binary_size     = find_kernel_size(ktype); 
  // copy the kernel binary to kernload-start 
  copyKernelTo(kernel_binary_size, kernel_load_addr, ktype);   
  uint64 kernel_entry           = find_kernel_entry_addr(ktype);
  
  /* CSE 536: Write the correct kernel entry point */
  w_mepc((uint64) kernel_entry);
}

// entry.S jumps here in machine mode on stack0.
void start()
{
  /* CSE 536: Define the system information table's location. */
  // SYSINFOADDR
  sys_info_ptr = (struct sys_info*) 0x80080000;

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid();
  w_tp(id);

  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // disable paging
  w_satp(0);

  /* CSE 536: Unless kernelpmp[1-2] booted, allow all memory 
   * regions to be accessed in S-mode. */ 
  #if !defined(KERNELPMP1) || !defined(KERNELPMP2)
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
  #endif

  /* CSE 536: With kernelpmp1, isolate upper 10MBs using TOR */ 
  #if defined(KERNELPMP1)
  // bootloader-start: 0x80000000 + 117MB
    // use 54 bits
    w_pmpaddr0(0x21D40000);
    // sets all permission bits (read, write, and execute) and the A field to the top-of-range.
    w_pmpcfg0(0xf);
  #endif

  /* CSE 536: With kernelpmp2, isolate 118-120 MB and 122-126 MB using NAPOT */ 
  #if defined(KERNELPMP2)
  // set up access until 118 MB
    w_pmpaddr0(0x21D80000);
    // 118-120MB, isolated
    w_pmpaddr1(0x21DBFFFF);
    // 120-122MB, start 120MB, decimal: 2273312768
    w_pmpaddr2(0x21E3FFFF);
    // 122-126MB, isolated, start 122MB
    w_pmpaddr3(0x43D7FFFF);
    //126-128MB
    w_pmpaddr4(0x21FBFFFF);
    // 0f: read, write, execute, f is 16, which means have all access, 18, 1 means use NAPTO entry
    w_pmpcfg0(0x1f181f180f);
  #endif

  /* CSE 536: Verify if the kernel is untampered for secure boot */
  //if (!is_secure_boot()) {
    /* Skip loading since we should have booted into a recovery kernel 
     * in the function is_secure_boot() */
    goto out;
  //}

  if (is_secure_boot()){
    setupKernel(NORMAL);
  }else{
    setupKernel(RECOVERY);
  }
  
 
 // out:
  /* CSE 536: Provide system information to the kernel. */
  sys_info_ptr -> bl_start = 0x80000000;
  sys_info_ptr -> bl_end = 0x80065848;
  // starting and ending address of the DRAM
  sys_info_ptr -> dr_start = 0x80065849;
  sys_info_ptr -> dr_end = 0x88000000;

  /* CSE 536: Send the observed hash value to the kernel (using sys_info_ptr) */

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // return address fix
  uint64 addr = (uint64) panic;
  asm volatile("mv ra, %0" : : "r" (addr));

  // switch to supervisor mode and jump to main().
  asm volatile("mret");
}
