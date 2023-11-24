#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "vm_state.h";

enum exe_mode current_exe_mode;
struct vm_virtual_state vm_state;

struct instruction{
    uint32 op;
    uint32 rd;
    uint32 funct3;
    uint32 rs1;
    uint32 uimm;
};

struct instruction retrieve_trap_instruction(void);

// In your ECALL, add the following for prints
// struct proc* p = myproc();
// printf("(EC at %p)\n", p->trapframe->epc);

void trap_and_emulate(void) {
    /* Comes here when a VM tries to execute a supervisor instruction. */

    /* Retrieve all required values from the instruction */
    uint64 addr = r_sepc();
    struct instruction trap_instruction = retrieve_trap_instruction();

    /* Print the statement */
    printf("(PI at %p) op = %x, rd = %x, funct3 = %x, rs1 = %x, uimm = %x\n", 
                addr, trap_instruction.op, trap_instruction.rd, trap_instruction.funct3, trap_instruction.rs1, trap_instruction.uimm);
}

uint32 get_coded_instruction(void);
struct instruction get_decoded_instruction(uint32 coded_instruction);

struct instruction retrieve_trap_instruction(void){
    uint64 coded_instruction = get_coded_instruction();
    return get_decoded_instruction(coded_instruction);
}

uint32 get_coded_instruction(void){
    struct proc *p = myproc();
    char *buffer = kalloc();
    uint64 va = r_sepc();
    copyin(p->pagetable, buffer, va, PGSIZE);
    return *((uint32*) buffer);
}

struct instruction get_decoded_instruction(uint32 coded_instruction){
    struct instruction instruct;
    // reference the  RISC-V Instruction Layout
    instruct.op = coded_instruction % 128;
    coded_instruction >>= 7;
    instruct.rd = coded_instruction % 32;
    coded_instruction >>= 5;
    instruct.funct3 = coded_instruction % 8;
    coded_instruction >>= 3;
    instruct.rs1 = coded_instruction % 32;
    coded_instruction >>= 5;
    instruct.uimm = coded_instruction;

    return instruct;
}

void trap_and_emulate_init(void) {
    /* Create and initialize all state for the VM */
    vm_state = get_vm_state();
    current_exe_mode = MACHINE;
}