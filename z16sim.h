#ifndef Z16SIM_H
#define Z16SIM_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <iostream>

class z16sim {
public:
    static constexpr int MEM_SIZE = 65536;  // 64KB memory

    z16sim();

    void loadMemoryFromFile(const char *filename);

    bool cycle();

private:
    // Global simulated memory and register file.
    unsigned char memory[MEM_SIZE];
    uint16_t regs[8];      // 8 registers (16-bit each): x0, x1, x2, x3, x4, x5, x6, x7
    uint16_t pc = 0;       // Program counter (16-bit)

    // Register ABI names for display (x0 = t0, x1 = ra, x2 = sp, x3 = s0, x4 = s1, x5 = t1, x6 = a0, x7 = a1)
    static constexpr const char *regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

    void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
    int executeInstruction(uint16_t inst);
};

#endif //Z16SIM_H
