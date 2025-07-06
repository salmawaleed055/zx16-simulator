#ifndef Z16SIM_H
#define Z16SIM_H

#include <cstdint>
#include <string>
#include <unordered_map> // Only needed if you use it for regMap
#include <vector>        // Only needed if you use it for memory or other dynamic arrays

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
    bool verbose; // Added for controlling messages like "Loaded X bytes"
    int infinityCheck[4]; // Moved into class, consider its purpose for interactive vs. batch

    // Register name mappings (static member, initialized in .cpp)
    static const char* regNames[NUM_REGS];
    void setReg(uint8_t reg_idx, uint16_t value); // Added this helper
    // std::unordered_map<std::string, int> regMap; // If you plan to use this for ABI name lookup

    // Helper methods (commented out if not used or if you implement them)
    // void initializeRegisterMap();
    // int getRegisterIndex(const std::string& regName);

public:
    z16sim(); // Constructor
    void dumpRegisters() const;
    void loadMemoryFromFile(const char* filename); // No prompts from here
    bool cycle(); // Executes one instruction cycle, returns true to continue, false to halt
    int executeInstruction(uint16_t inst); // Internal execution logic, returns 0 to signal halt
    void reset();

    // Debugging and control setters/getters
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { this->pc = new_pc; } // Added this line explicitly
    void setDebug(bool d);
    bool isDebug() const; // Optional getter

    void setVerbose(bool val);
    bool isVerbose() const; // Optional getter

    // Disassembler method
    void disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize);

    // Other potentially useful getters for frontend
    uint16_t getReg(int index) const {
        if (index >= 0 && index < NUM_REGS) return regs[index];
        return 0; // Or throw an error
    }
    unsigned char getMemByte(uint16_t addr) const {
        if (addr < MEM_SIZE) return memory[addr];
        return 0; // Or throw an error
    }
};

#endif // Z16SIM_H

