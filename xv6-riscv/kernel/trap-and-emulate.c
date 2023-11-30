#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "vm_state.h"
#include "read_write_registers.h"

enum exe_mode current_exe_mode;
struct vm_virtual_state vm_state;

struct instruction{
    uint32 op;
    uint32 rd;
    uint32 funct3;
    uint32 rs1;
    uint32 uimm;
    uint64 addr;
};

struct instruction retrieve_trap_instruction(void);
uint32 trap_instruction_emulation(struct instruction *trapInstruction);

// In your ECALL, add the following for prints
// struct proc* p = myproc();
// printf("(EC at %p)\n", p->trapframe->epc);

uint32 trap_and_emulate_ecall(void){
    //struct proc *p = myproc();
    struct instruction trap_instruction = retrieve_trap_instruction();
    //printf("(EC at %p)\n", p->trapframe->epc);
    printf("(EC at %p)\n", trap_instruction.addr);
    return trap_instruction_emulation(&trap_instruction);
}

uint32 trap_and_emulate(void) {
    /* Comes here when a VM tries to execute a supervisor instruction. */

    /* Retrieve all required values from the instruction */
    //uint64 addr = r_sepc();
    struct instruction trap_instruction = retrieve_trap_instruction();

    /* Print the statement */
    printf("(PI at %p) op = %x, rd = %x, funct3 = %x, rs1 = %x, uimm = %x\n", 
                trap_instruction.addr, trap_instruction.op, trap_instruction.rd, trap_instruction.funct3, trap_instruction.rs1, trap_instruction.uimm);

    return trap_instruction_emulation(&trap_instruction);
}

uint32 get_coded_instruction(void);
struct instruction get_decoded_instruction(uint32 coded_instruction);

struct instruction retrieve_trap_instruction(void){
    uint64 coded_instruction = get_coded_instruction();
    return get_decoded_instruction(coded_instruction);
}

uint32 get_coded_instruction(void){
    struct proc *p = myproc();
    uint64 va = r_sepc();
    char *buffer = kalloc();
    copyin(p->pagetable, buffer, va, PGSIZE);
    return *((uint32*) buffer);
}

struct instruction get_decoded_instruction(uint32 coded_instruction){
    struct instruction instruct;
    instruct.addr = r_sepc();
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

uint32 emulate_csrr(struct instruction* trapInstruction);
uint32 emulate_csrw(struct instruction* trapInstruction);
uint32 emulate_mret(struct instruction* trapInstruction);
uint32 emulate_sret(struct instruction* trapInstruction);
uint32 emulate_ecall(struct instruction* trapInstruction);
uint32 instruction_valid_read(uint32 authenticate);
uint32 instruction_valid_write(uint32 authenticate);

uint32 trap_instruction_emulation(struct instruction *trapInstruction){
    if(trapInstruction->funct3 == 0x1){
    // csrw
        return emulate_csrw(trapInstruction);
    }else if (trapInstruction->funct3 == 0x2){
    // csrr
        return emulate_csrr(trapInstruction);
    }else if (trapInstruction->uimm == 0x302){
    // mret
        return emulate_mret(trapInstruction);
    }else if (trapInstruction->uimm == 0x102){
    // sret
        return emulate_sret(trapInstruction);
    }else if (trapInstruction->uimm == 0x0){
        return emulate_ecall(trapInstruction);
    }else{
        panic("This instruction is not emulated\n");
        return 0;
    }
}

uint32 emulate_csrr(struct instruction* trapInstruction){
    struct vm_reg *regis = get_privilege_reg(&vm_state, trapInstruction->uimm);
    // if pass authentication
    if (instruction_valid_read(regis->authenticate)){
        write_registers(trapInstruction->rd, regis->val);
        //return 0;
    }else{
        return 0;
    }
    return 1;
}

uint32 emulate_csrw(struct instruction* trapInstruction){
    struct vm_reg *regis = get_privilege_reg(&vm_state, trapInstruction->uimm);
    if(instruction_valid_write(regis->authenticate)){
        regis->val = read_registers(trapInstruction->rs1);
        // kill proc when mvendorid is 0x0
        return regis->code != 0xf11 || regis->val != 0x0 ? 1 : 0;
    }else{
        return 0;
    }
}
// q5
void copy_for_US_exe(void);

uint32 emulate_mret(struct instruction* trapInstruction){
    struct proc *p = myproc();
    // start of MACHINE trap: 0x300, mstatus
    struct vm_reg *regis = get_privilege_reg(&vm_state, 0x300);
    uint64 mask = regis->val & MSTATUS_MPP_MASK;
    // if in mahcine mode
    if(current_exe_mode == MACHINE && (mask & MSTATUS_MPP_S) == MSTATUS_MPP_S){
        current_exe_mode = SUPERVISOR;
        // jump to mepc register
        struct vm_reg *mepc = get_privilege_reg(&vm_state, 0x341);
        p->trapframe->epc = mepc->val - 4;

        copy_for_US_exe();
    }else{
        return 0;
    }
    return 1;
}

void pmp_region_preparation(void); 

void copy_for_US_exe(void){
    // backup page table
    struct proc *p = myproc();
    p->vm_pagetable = uvmcreate();
    //uvmcopy(p->pagetable, p->vm_pagetable, p->sz);
    p->pagetable = uvmcreate();
    
    pmp_region_preparation();
}

// q5
uint64 PTE_permission(uint64 pmp_authenticate);
void page_copy(uint64 sz, uint64 permission);

void pmp_region_preparation(void) {
    //struct proc *p = myproc();
    struct vm_reg *pmpconfig0 = get_privilege_reg(&vm_state, 0x3a0);
    struct vm_reg *pmpaddr0 = get_privilege_reg(&vm_state, 0x3b0);

    uint64 pmp_authentication = pmpconfig0->val & 3;
    uint64 permission = PTE_permission(pmp_authentication);
    uint64 upper = (pmpaddr0->val << 2);
    // for a created VM, the VMM automatically provides a 4MB memory region from 0x80000000 - 0x80400000.
    if(upper < 0x80000000) {
        return;
    }

    page_copy(upper, permission);
}

void page_copy(uint64 upper, uint64 permission){
    struct proc *p = myproc();

    for (uint64 start = 0x80000000; start<upper; start += PGSIZE){
        uint64 phys_addr = walkaddr(p->vm_pagetable, start);
        mappages(p->pagetable, start, PGSIZE, phys_addr, permission);
    }
}

uint64 PTE_permission(uint64 pmp_authenticate){
    if (pmp_authenticate == 1){
        return PTE_R;
    }else if(pmp_authenticate == 2){
        return PTE_W;
    }else{
        return PTE_U | PTE_R | PTE_W | PTE_X;
    }
}

uint32 emulate_sret(struct instruction* trapInstruction){
    struct proc *p = myproc();
    // sstatus
    struct vm_reg *regis = get_privilege_reg(&vm_state, 0x100);
    uint64 mask = regis->val & SSTATUS_SPP;
    // if in SUPERVISOR
    if(current_exe_mode == SUPERVISOR && mask == 0){
        current_exe_mode = USER;
        // jump to sepc register
        struct vm_reg *sepc = get_privilege_reg(&vm_state, 0x141);
        p->trapframe->epc = sepc->val - 4;
    }else{
        return 0;
    }
    return 1;
}

uint32 emulate_ecall(struct instruction* trapInstruction){
    struct proc *p = myproc();
    // stvel register
    struct vm_reg *stvec = get_privilege_reg(&vm_state, 0x105);
    // sepc register
    struct vm_reg *sepc = get_privilege_reg(&vm_state, 0x141);
    current_exe_mode = SUPERVISOR;
    // save epc so it can go back to it
    sepc->val = p->trapframe->epc;
    p->trapframe->epc = stvec->val - 4;
    
    return 1;
}

uint32 instruction_valid_read(uint32 authenticate){
    return current_exe_mode >= (authenticate >> 4);
}

uint32 instruction_valid_write(uint32 authenticate){
    return current_exe_mode >= (authenticate & 15);
}

void trap_and_emulate_init(void) {
    /* Create and initialize all state for the VM */
    vm_state = get_vm_state();
    current_exe_mode = MACHINE;
    printf("start emulating\n");
}