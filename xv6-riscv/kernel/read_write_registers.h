#include "types.h"

// https://five-embeddev.com/quickref/regs_abi.html
// UABI
void write_registers(uint64 code, uint64 val){
    struct proc *p = myproc();
        if(code == 0x1){
            p->trapframe->ra = val;}
        else if (code == 1){
            p->trapframe->ra = val;}
        else if (code == 2){
            p->trapframe->sp = val;
        }else if (code == 3)
        {
            p->trapframe->gp = val;
        }else if (code == 4){
            p->trapframe->tp = val;
        }else if (code == 5){
            p->trapframe->t0 = val;
        }else if (code == 6){
            p->trapframe->t1 = val;
        }else if (code == 7){
            p->trapframe->t2 = val;
        }else if (code == 8){
            p->trapframe->s0 = val;
        }else if (code == 9){
            p->trapframe->s1 = val;
        }else if (code == 10){
            p->trapframe->a0 = val;
        }else if (code == 11){
            p->trapframe->a1 = val;
        }else if (code == 12){
            p->trapframe->a2 = val;
        }else if (code == 13){
            p->trapframe->a3 = val;
        }else if (code == 14){
            p->trapframe->a4 = val;
        }else if (code == 15){
            p->trapframe->a5 = val;
        }else if (code == 16){
            p->trapframe->a6 = val;
        }else if (code == 17){
            p->trapframe->a7 = val;
        }else if (code == 18){
            p->trapframe->s2 = val;
        }else if (code == 19){
            p->trapframe->s3 = val;
        }else if (code == 20){
            p->trapframe->s4 = val;
        }else if (code == 21){
            p->trapframe->s5 = val;
        }else if (code == 22){
            p->trapframe->s6 = val;
        }else if (code == 23){
            p->trapframe->s7 = val;
        }else if (code == 24){
            p->trapframe->s8 = val;
        }else if (code == 25){
            p->trapframe->s9 = val;
        }else if (code == 26){
            p->trapframe->s10 = val;
        }else if (code == 27){
            p->trapframe->s11 = val;
        }else if (code == 28){
            p->trapframe->t3 = val;
        }else if (code == 29){
            p->trapframe->t4 = val;
        }else if (code == 30){
            p->trapframe->t5 = val;
        }else if (code == 31){
            p->trapframe->t6 = val;
        }
}

uint64 read_registers(uint64 code){
    struct proc *p = myproc();
        if(code == 0x1){
            return p->trapframe->ra;}
        else if (code == 1){
            return p->trapframe->ra;}
        else if (code == 2){
            return p->trapframe->sp;
        }else if (code == 3)
        {
            return p->trapframe->gp;
        }else if (code == 4){
            return p->trapframe->tp;
        }else if (code == 5){
            return p->trapframe->t0;
        }else if (code == 6){
            return p->trapframe->t1;
        }else if (code == 7){
            return p->trapframe->t2;
        }else if (code == 8){
            return p->trapframe->s0;
        }else if (code == 9){
            return p->trapframe->s1;
        }else if (code == 10){
            return p->trapframe->a0;
        }else if (code == 11){
            return p->trapframe->a1;
        }else if (code == 12){
            return p->trapframe->a2;
        }else if (code == 13){
            return p->trapframe->a3;
        }else if (code == 14){
            return p->trapframe->a4;
        }else if (code == 15){
            return p->trapframe->a5;
        }else if (code == 16){
            return p->trapframe->a6;
        }else if (code == 17){
            return p->trapframe->a7;
        }else if (code == 18){
            return p->trapframe->s2;
        }else if (code == 19){
            return p->trapframe->s3;
        }else if (code == 20){
            return p->trapframe->s4;
        }else if (code == 21){
            return p->trapframe->s5;
        }else if (code == 22){
            return p->trapframe->s6;
        }else if (code == 23){
            return p->trapframe->s7;
        }else if (code == 24){
            return p->trapframe->s8;
        }else if (code == 25){
            return p->trapframe->s9;
        }else if (code == 26){
            return p->trapframe->s10;
        }else if (code == 27){
            return p->trapframe->s11;
        }else if (code == 28){
            return p->trapframe->t3;
        }else if (code == 29){
            return p->trapframe->t4;
        }else if (code == 30){
            return p->trapframe->t5;
        }else if (code == 31){
            return p->trapframe->t6;
        }
}