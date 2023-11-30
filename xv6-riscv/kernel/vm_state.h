#include "types.h"

#define PRIVILEGE_REGISTERS 38

enum exe_mode {USER, SUPERVISOR, MACHINE};

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int code;
    uint64 val;
    uint32 authenticate; 
    // the leftmost digit: read
    // the rightmost digit: write
};

// Keep the virtual state of the VM's privileged registers
struct vm_virtual_state {
    struct vm_reg privilege_registers[PRIVILEGE_REGISTERS];
};

void setup_privilege_regs(struct vm_virtual_state *state);

struct vm_virtual_state get_vm_state(void) {
    struct vm_virtual_state vm_state;

    setup_privilege_reg(&vm_state);

    return vm_state;
}

void setup_privilege_regs(struct vm_virtual_state *state) {
    // Machine trap handling registers
    state->privilege_registers[0] = (struct vm_reg){.code = 0x340, .authenticate = 0x22, .val = 0};
    state->privilege_registers[1] = (struct vm_reg){.code = 0x341, .authenticate = 0x22, .val = 0};
    state->privilege_registers[2] = (struct vm_reg){.code = 0x342, .authenticate = 0x22, .val = 0};
    state->privilege_registers[3] = (struct vm_reg){.code = 0x343, .authenticate = 0x22, .val = 0};
    state->privilege_registers[4] = (struct vm_reg){.code = 0x344, .authenticate = 0x22, .val = 0};
    state->privilege_registers[5] = (struct vm_reg){.code = 0x34a, .authenticate = 0x22, .val = 0};
    state->privilege_registers[6] = (struct vm_reg){.code = 0x34b, .authenticate = 0x22, .val = 0};
    // Machine trap setup registers
    state->privilege_registers[7] = (struct vm_reg){.code = 0x300, .authenticate = 0x22, .val = 0x1800};
    state->privilege_registers[8] = (struct vm_reg){.code = 0xf10, .authenticate = 0x22, .val = 0};
    state->privilege_registers[9] = (struct vm_reg){.code = 0x302, .authenticate = 0x22, .val = 0};
    state->privilege_registers[10] = (struct vm_reg){.code = 0x303, .authenticate = 0x22, .val = 0};
    state->privilege_registers[11] = (struct vm_reg){.code = 0x304, .authenticate = 0x22, .val = 0};
    state->privilege_registers[12] = (struct vm_reg){.code = 0x305, .authenticate = 0x22, .val = 0};
    state->privilege_registers[13] = (struct vm_reg){.code = 0x306, .authenticate = 0x22, .val = 0};
    state->privilege_registers[14] = (struct vm_reg){.code = 0x310, .authenticate = 0x22, .val = 0};
    // Machine information registers
    state->privilege_registers[15] = (struct vm_reg){.code = 0xf11, .authenticate = 0x02, .val = 0x637365353336};
    state->privilege_registers[16] = (struct vm_reg){.code = 0xf12, .authenticate = 0x22, .val = 0};
    state->privilege_registers[17] = (struct vm_reg){.code = 0xf13, .authenticate = 0x22, .val = 0};
    state->privilege_registers[18] = (struct vm_reg){.code = 0xf14, .authenticate = 0x22, .val = 0};
    state->privilege_registers[19] = (struct vm_reg){.code = 0xf15, .authenticate = 0x22, .val = 0};
    // Supervisor page table register
    state->privilege_registers[20] = (struct vm_reg){.code = 0x180, .authenticate = 0x11, .val = 0};
    // Supervisor trap setup registers
    state->privilege_registers[21] = (struct vm_reg){.code = 0x100, .authenticate = 0x11, .val = 0};
    state->privilege_registers[22] = (struct vm_reg){.code = 0x104, .authenticate = 0x11, .val = 0};
    state->privilege_registers[23] = (struct vm_reg){.code = 0x105, .authenticate = 0x11, .val = 0};
    state->privilege_registers[24] = (struct vm_reg){.code = 0x106, .authenticate = 0x11, .val = 0};
    // Supervisor trap handling registers
    state->privilege_registers[25] = (struct vm_reg){.code = 0x140, .authenticate = 0x11, .val = 0};
    state->privilege_registers[26] = (struct vm_reg){.code = 0x141, .authenticate = 0x11, .val = 0};
    state->privilege_registers[27] = (struct vm_reg){.code = 0x142, .authenticate = 0x11, .val = 0};
    state->privilege_registers[28] = (struct vm_reg){.code = 0x143, .authenticate = 0x11, .val = 0};
    state->privilege_registers[29] = (struct vm_reg){.code = 0x144, .authenticate = 0x11, .val = 0};
    // User trap handling registers
    state->privilege_registers[30] = (struct vm_reg){.code = 0x040, .authenticate = 0x00, .val = 0};
    state->privilege_registers[31] = (struct vm_reg){.code = 0x041, .authenticate = 0x00, .val = 0};
    state->privilege_registers[32] = (struct vm_reg){.code = 0x042, .authenticate = 0x00, .val = 0};
    state->privilege_registers[33] = (struct vm_reg){.code = 0x043, .authenticate = 0x00, .val = 0};
    state->privilege_registers[34] = (struct vm_reg){.code = 0x044, .authenticate = 0x00, .val = 0};
    // User trap setup registers
    state->privilege_registers[35] = (struct vm_reg){.code = 0x000, .authenticate = 0x00, .val = 0};
    state->privilege_registers[36] = (struct vm_reg){.code = 0x004, .authenticate = 0x00, .val = 0};
    state->privilege_registers[37] = (struct vm_reg){.code = 0x005, .authenticate = 0x00, .val = 0};
}

struct vm_reg* get_privilege_reg(struct vm_virtual_state* state, int code) {
    for(int i = 0;i < PRIVILEGE_REGISTERS;i++) {
        if(state->privilege_registers[i].code == code) {
            return &state->privilege_registers[i];
        }
    }

    return NULL;
}