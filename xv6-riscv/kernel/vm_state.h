#include "types.h"

#define PRIVIREGSAMOUNT 38

enum exe_mode {USER, SUPERVISOR, MACHINE};

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int code;
    uint64 val;
    uint32 authenticate; 
    // the leftmost digit present the auth of read, and the rightmost digit presetn the auth of write
};

// Keep the virtual state of the VM's privileged registers
struct vm_virtual_state {
    struct vm_reg privi_regs[PRIVIREGSAMOUNT];
};

void setup_privi_regs(struct vm_virtual_state *state);

struct vm_virtual_state get_vm_state(void) {
    struct vm_virtual_state vm_state;

    setup_privi_regs(&vm_state);

    return vm_state;
}

void setup_privi_regs(struct vm_virtual_state *state) {
    // Machine trap handling registers
    state->privi_regs[0] = (struct vm_reg){.code = 0x340, .authenticate = 0x22, .val = 0};
    state->privi_regs[1] = (struct vm_reg){.code = 0x341, .authenticate = 0x22, .val = 0};
    state->privi_regs[2] = (struct vm_reg){.code = 0x342, .authenticate = 0x22, .val = 0};
    state->privi_regs[3] = (struct vm_reg){.code = 0x343, .authenticate = 0x22, .val = 0};
    state->privi_regs[4] = (struct vm_reg){.code = 0x344, .authenticate = 0x22, .val = 0};
    state->privi_regs[5] = (struct vm_reg){.code = 0x34a, .authenticate = 0x22, .val = 0};
    state->privi_regs[6] = (struct vm_reg){.code = 0x34b, .authenticate = 0x22, .val = 0};
    // Machine trap setup registers
    state->privi_regs[7] = (struct vm_reg){.code = 0x300, .authenticate = 0x22, .val = 0x1800};
    state->privi_regs[8] = (struct vm_reg){.code = 0xf10, .authenticate = 0x22, .val = 0};
    state->privi_regs[9] = (struct vm_reg){.code = 0x302, .authenticate = 0x22, .val = 0};
    state->privi_regs[10] = (struct vm_reg){.code = 0x303, .authenticate = 0x22, .val = 0};
    state->privi_regs[11] = (struct vm_reg){.code = 0x304, .authenticate = 0x22, .val = 0};
    state->privi_regs[12] = (struct vm_reg){.code = 0x305, .authenticate = 0x22, .val = 0};
    state->privi_regs[13] = (struct vm_reg){.code = 0x306, .authenticate = 0x22, .val = 0};
    state->privi_regs[14] = (struct vm_reg){.code = 0x310, .authenticate = 0x22, .val = 0};
    // Machine information registers
    state->privi_regs[15] = (struct vm_reg){.code = 0xf11, .authenticate = 0x02, .val = 0x637365353336};
    state->privi_regs[16] = (struct vm_reg){.code = 0xf12, .authenticate = 0x22, .val = 0};
    state->privi_regs[17] = (struct vm_reg){.code = 0xf13, .authenticate = 0x22, .val = 0};
    state->privi_regs[18] = (struct vm_reg){.code = 0xf14, .authenticate = 0x22, .val = 0};
    state->privi_regs[19] = (struct vm_reg){.code = 0xf15, .authenticate = 0x22, .val = 0};
    // Supervisor page table register
    state->privi_regs[20] = (struct vm_reg){.code = 0x180, .authenticate = 0x11, .val = 0};
    // Supervisor trap setup registers
    state->privi_regs[21] = (struct vm_reg){.code = 0x100, .authenticate = 0x11, .val = 0};
    state->privi_regs[22] = (struct vm_reg){.code = 0x104, .authenticate = 0x11, .val = 0};
    state->privi_regs[23] = (struct vm_reg){.code = 0x105, .authenticate = 0x11, .val = 0};
    state->privi_regs[24] = (struct vm_reg){.code = 0x106, .authenticate = 0x11, .val = 0};
    // Supervisor trap handling registers
    state->privi_regs[25] = (struct vm_reg){.code = 0x140, .authenticate = 0x11, .val = 0};
    state->privi_regs[26] = (struct vm_reg){.code = 0x141, .authenticate = 0x11, .val = 0};
    state->privi_regs[27] = (struct vm_reg){.code = 0x142, .authenticate = 0x11, .val = 0};
    state->privi_regs[28] = (struct vm_reg){.code = 0x143, .authenticate = 0x11, .val = 0};
    state->privi_regs[29] = (struct vm_reg){.code = 0x144, .authenticate = 0x11, .val = 0};
    // User trap handling registers
    state->privi_regs[30] = (struct vm_reg){.code = 0x040, .authenticate = 0x00, .val = 0};
    state->privi_regs[31] = (struct vm_reg){.code = 0x041, .authenticate = 0x00, .val = 0};
    state->privi_regs[32] = (struct vm_reg){.code = 0x042, .authenticate = 0x00, .val = 0};
    state->privi_regs[33] = (struct vm_reg){.code = 0x043, .authenticate = 0x00, .val = 0};
    state->privi_regs[34] = (struct vm_reg){.code = 0x044, .authenticate = 0x00, .val = 0};
    // User trap setup registers
    state->privi_regs[35] = (struct vm_reg){.code = 0x000, .authenticate = 0x00, .val = 0};
    state->privi_regs[36] = (struct vm_reg){.code = 0x004, .authenticate = 0x00, .val = 0};
    state->privi_regs[37] = (struct vm_reg){.code = 0x005, .authenticate = 0x00, .val = 0};
}

struct vm_reg* get_privilege_reg(struct vm_virtual_state* state, int code) {
    for(int i = 0;i < PRIVIREGSAMOUNT;i++) {
        if(state->privi_regs[i].code == code) {
            return &state->privi_regs[i];
        }
    }

    return NULL;
}