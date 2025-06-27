// #ifndef Z16SIM_H
// #define Z16SIM_H
//
// #include <cstdio>
// #include <cstdlib>
// #include <cstdint>
// #include <cstring>
// #include <cctype>
// #include <iostream>
//
// class z16sim {
// public:
//     static constexpr int MEM_SIZE = 65536;  // 64KB memory
//
//     z16sim();
//
//     void loadMemoryFromFile(const char *filename);
//     uint8_t memory[MEM_SIZE];
//     bool cycle();
//     // Public getters/setters for test access
//     uint16_t getPC() const { return pc; }
//     void setPC(uint16_t new_pc) { pc = new_pc; } // Useful if you want to set initial PC for a test
//     uint16_t getReg(uint8_t reg_idx) const {
//         if (reg_idx < 8) return regs[reg_idx];
//         return 0; // Or throw an error for invalid index
//     }
//     void setReg(uint8_t reg_idx, uint16_t val) {
//         if (reg_idx < 8) regs[reg_idx] = val;
//         // Else: handle error for invalid index
//     }
// private:
//     // Global simulated memory and register file.
//     //unsigned char memory[MEM_SIZE];
//     uint16_t regs[8];      // 8 registers (16-bit each): x0, x1, x2, x3, x4, x5, x6, x7
//     uint16_t pc = 0;       // Program counter (16-bit)
//
//     // Register ABI names for display (x0 = t0, x1 = ra, x2 = sp, x3 = s0, x4 = s1, x5 = t1, x6 = a0, x7 = a1)
//     static constexpr const char *regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};
//
//     void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
//     int executeInstruction(uint16_t inst);
//     // void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
//     // int executeInstruction(uint16_t inst);
//     bool updatePC(uint16_t new_pc, const char* instruction_name);
// };
//
// #endif //Z16SIM_H

#ifndef Z16SIM_H
#define Z16SIM_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <iostream> // For std::cout in case you use it for debugging within the class

class z16sim {
public:
    static constexpr int MEM_SIZE = 65536;  // 64KB memory
    static constexpr int NUM_REGS = 8;     // 8 registers

    // Register ABI names for display (made public for main.cpp access)
    static constexpr const char *regNames[NUM_REGS] = {"x0", "x1", "x2", "x3", "x4", "x5", "a0", "a1"};
    // Note: I've updated the regNames to match "x0"..."x5", "a0", "a1" as per the ecall descriptions.
    // If your registers are strictly "x0"..."x7", then please revert a0/a1 to x6/x7.

    z16sim();

    void loadMemoryFromFile(const char *filename);
    uint8_t memory[MEM_SIZE]; // Public for external test inspection if needed (e.g., verifying stores)
    bool cycle();

    // Public getters/setters for test access
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { pc = new_pc; } // Useful if you want to set initial PC for a test

    uint16_t getReg(uint8_t reg_idx) const {
        if (reg_idx < NUM_REGS) return regs[reg_idx];
        // For robustness, you might want to log an error or throw an exception here
        // std::cerr << "Error: Attempted to access invalid register index " << (int)reg_idx << std::endl;
        return 0; // Return 0 for invalid index
    }
    void setReg(uint8_t reg_idx, uint16_t val) {
        if (reg_idx < NUM_REGS) regs[reg_idx] = val;
        // For robustness, you might want to log an error or throw an exception here
        // else { std::cerr << "Error: Attempted to write to invalid register index " << (int)reg_idx << std::endl; }
    }

private:
    uint16_t regs[NUM_REGS]; // 8 registers (16-bit each)
    uint16_t pc = 0;       // Program counter (16-bit)

    void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize);
    int executeInstruction(uint16_t inst);
    bool updatePC(uint16_t new_pc, const char* instruction_name);
};

#endif //Z16SIM_H