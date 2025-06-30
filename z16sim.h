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
    static constexpr int MEM_SIZE = 65536; // 64KB memory
    static constexpr int NUM_REGS = 8;

    // Register ABI names for display
    static constexpr const char *regNames[NUM_REGS] = {
        "x0", "x1", "x2", "x3", "x4", "x5", "a0", "a1"
    };

    z16sim();

    void reset(); // Resets registers and PC
    void loadMemoryFromFile(const char *filename);
    bool cycle(); // Executes a single instruction cycle

    // Getters and setters
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { pc = new_pc; }

    uint16_t getReg(uint8_t reg_idx) const {
        return (reg_idx < NUM_REGS) ? regs[reg_idx] : 0;
    }

    void setReg(uint8_t reg_idx, uint16_t val) {
        if (reg_idx < NUM_REGS) regs[reg_idx] = val;
    }

    uint8_t memory[MEM_SIZE]; // Public for inspection during testing
    void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);

private:
    uint16_t regs[NUM_REGS] = {0};
    uint16_t pc = 0;


    int executeInstruction(uint16_t inst);
    bool updatePC(uint16_t new_pc, const char* instruction_name);
};

#endif // Z16SIM_H
