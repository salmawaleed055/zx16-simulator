#ifndef Z16SIM_H
#define Z16SIM_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
//#include <SFML/Graphics.hpp>
//#include "graphics.h"

class z16sim {
private:
    // Constants
    static const size_t MEM_SIZE = 65536;
    static const int NUM_REGS = 8;
    static const int RA_REG = 1; // ra register index (x1)

    // Simulator state
    uint16_t regs[NUM_REGS];
    uint16_t pc;
    unsigned char memory[MEM_SIZE];
    bool debug;
    bool verbose;
    int infinityCheck[4];

    // Register name mappings (static member, initialized in .cpp)
    static const char* regNames[NUM_REGS];
    void setReg(uint8_t reg_idx, uint16_t value);

public:
    z16sim(); // Constructor
    void dumpRegisters() const;
    void loadMemoryFromFile(const char* filename);
    bool cycle();
    int executeInstruction(uint16_t inst);
    void reset();

    // Debugging and control setters/getters
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { this->pc = new_pc; }
    void setDebug(bool d);
    bool isDebug() const;

    void setVerbose(bool val);
    bool isVerbose() const;

    void disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize);

    uint16_t getReg(int index) const {
        if (index >= 0 && index < NUM_REGS) return regs[index];
        return 0;
    }
    unsigned char getMemByte(uint16_t addr) const {
        if (addr < MEM_SIZE) return memory[addr];
        return 0;
    }


    //Graphics graphics; // instance of graphics class

};

#endif // Z16SIM_H

