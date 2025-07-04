#ifndef Z16SIM_H
#define Z16SIM_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class z16sim {
private:
    // Constants
    static const int MEM_SIZE = 65536;
    static const int NUM_REGS = 8;
    static const int RA_REG = 1; // ra register index

    // Simulator state
    uint16_t regs[NUM_REGS];
    uint16_t pc;
    unsigned char memory[MEM_SIZE];
    bool debug;

    // Register name mappings
    static const char* regNames[NUM_REGS];
    std::unordered_map<std::string, int> regMap;

    // Assembler support
    std::unordered_map<std::string, uint16_t> labels;
    std::vector<std::pair<uint16_t, std::string> > unresolved_labels; // (address, label_name)

    // Helper methods
    void initializeRegisterMap();
    int getRegisterIndex(const std::string& regName);

    bool updatePC(uint16_t new_pc, const char* instruction_name);

public:
    z16sim();
    void dumpRegisters() const;
    void loadMemoryFromFile(const char* filename); // For binary files
    bool cycle();
    int executeInstruction(uint16_t inst);
    void reset();
    void disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize);
    uint16_t getPC() const { return pc; }
    void setDebug(bool d) { debug = d; }
};

#endif // Z16SIM_H

