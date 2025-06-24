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
    uint8_t memory[MEM_SIZE];
    bool cycle();
    // Public getters/setters for test access
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { pc = new_pc; } // Useful if you want to set initial PC for a test
    uint16_t getReg(uint8_t reg_idx) const {
        if (reg_idx < 8) return regs[reg_idx];
        return 0; // Or throw an error for invalid index
    }
    void setReg(uint8_t reg_idx, uint16_t val) {
        if (reg_idx < 8) regs[reg_idx] = val;
        // Else: handle error for invalid index
    }
private:
    // Global simulated memory and register file.
    //unsigned char memory[MEM_SIZE];
    uint16_t regs[8];      // 8 registers (16-bit each): x0, x1, x2, x3, x4, x5, x6, x7
    uint16_t pc = 0;       // Program counter (16-bit)

    // Register ABI names for display (x0 = t0, x1 = ra, x2 = sp, x3 = s0, x4 = s1, x5 = t1, x6 = a0, x7 = a1)
    static constexpr const char *regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

    void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
    int executeInstruction(uint16_t inst);
    // void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
    // int executeInstruction(uint16_t inst);
    bool updatePC(uint16_t new_pc, const char* instruction_name);
};

#endif //Z16SIM_H
