#include "types.h"; 

enum exe_mode {MACHINE, SUPERVISOR, USER};

// Struct to keep VM registers (Sample; feel free to change.)
struct vm_reg {
    int code;
    enum exe_mode mode;
    uint64 val;
};

// Keep the virtual state of the VM's privileged registers
struct vm_virtual_state {
    // User trap setup
    // User trap handling
    // Supervisor trap setup
    // User trap handling
    // Supervisor page table register
    // Machine information registers
    // Machine trap setup registers
    // Machine trap handling registers

    //struct vm_reg tmp;
    struct vm_reg privilege_registers[33];
};

void setup_registers(void);

struct vm_virtual_state get_vm_state(void) {
    struct vm_virtual_state state;
    setup_registers(&state);
    return state;
}

void setup_registers(struct vm_virtual_state *state) {
    // Machine trap handling registers
    state->privilege_registers[0] = (struct vm_reg){.code = 0x340, .mode = MACHINE, .val = 0};
    state->privilege_registers[1] = (struct vm_reg){.code = 0x341, .mode = MACHINE, .val = 0};
    state->privilege_registers[2] = (struct vm_reg){.code = 0x342, .mode = MACHINE, .val = 0};
    state->privilege_registers[3] = (struct vm_reg){.code = 0x343, .mode = MACHINE, .val = 0};
    state->privilege_registers[4] = (struct vm_reg){.code = 0x344, .mode = MACHINE, .val = 0};
    state->privilege_registers[5] = (struct vm_reg){.code = 0x34a, .mode = MACHINE, .val = 0};
    state->privilege_registers[6] = (struct vm_reg){.code = 0x34b, .mode = MACHINE, .val = 0};
    // Machine Trap Setup
    state->privilege_registers[7] = (struct vm_reg){.code = 0x300, .mode = MACHINE, .val = 0};
    state->privilege_registers[8] = (struct vm_reg){.code = 0x301, .mode = MACHINE, .val = 0};
    state->privilege_registers[9] = (struct vm_reg){.code = 0x302, .mode = MACHINE, .val = 0};
    state->privilege_registers[10] = (struct vm_reg){.code = 0x303, .mode = MACHINE, .val = 0};
    state->privilege_registers[11] = (struct vm_reg){.code = 0x304, .mode = MACHINE, .val = 0};
    state->privilege_registers[12] = (struct vm_reg){.code = 0x305, .mode = MACHINE, .val = 0};
    state->privilege_registers[13] = (struct vm_reg){.code = 0x306, .mode = MACHINE, .val = 0};
    state->privilege_registers[14] = (struct vm_reg){.code = 0x310, .mode = MACHINE, .val = 0};
    // Machine information registers
    // mvendorid register, which you should initialize to the hexadecimal code of “cse536”
    state->privilege_registers[15] = (struct vm_reg){.code = 0xf11, .mode = MACHINE, .val = 0x637365353336};
    state->privilege_registers[16] = (struct vm_reg){.code = 0xf12, .mode = MACHINE, .val = 0};
    state->privilege_registers[17] = (struct vm_reg){.code = 0xf13, .mode = MACHINE, .val = 0};
    state->privilege_registers[18] = (struct vm_reg){.code = 0xf14, .mode = MACHINE, .val = 0};
    state->privilege_registers[19] = (struct vm_reg){.code = 0xf15, .mode = MACHINE, .val = 0};
    // Machine physical memory protection registers, there are 64 registers



    // Supervisor page table register, supervisor protection and translation
    state->privilege_registers[20] = (struct vm_reg){.code = 0x180, .mode = SUPERVISOR, .val = 0};

    // Supervisor trap setup
    state->privilege_registers[21] = (struct vm_reg){.code = 0x100, .mode = SUPERVISOR, .val = 0};
    state->privilege_registers[22] = (struct vm_reg){.code = 0x104, .mode = SUPERVISOR, .val = 0};
    state->privilege_registers[23] = (struct vm_reg){.code = 0x105, .mode = SUPERVISOR, .val = 0};
    state->privilege_registers[24] = (struct vm_reg){.code = 0x106, .mode = SUPERVISOR, .val = 0};
    // User trap handling
    state->privilege_registers[25] = (struct vm_reg){.code = 0x040, .mode = USER, .val = 0};
    state->privilege_registers[26] = (struct vm_reg){.code = 0x041, .mode = USER, .val = 0};
    state->privilege_registers[27] = (struct vm_reg){.code = 0x042, .mode = USER, .val = 0};
    state->privilege_registers[28] = (struct vm_reg){.code = 0x043, .mode = USER, .val = 0};
    state->privilege_registers[29] = (struct vm_reg){.code = 0x044, .mode = USER, .val = 0};
    // User trap setup
    state->privilege_registers[30] = (struct vm_reg){.code = 0x000, .mode = USER, .val = 0};
    state->privilege_registers[31] = (struct vm_reg){.code = 0x004, .mode = USER, .val = 0};
    state->privilege_registers[32] = (struct vm_reg){.code = 0x005, .mode = USER, .val = 0};

}