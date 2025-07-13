#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <stdexcept>

// Custom exception for memory access errors


class Memory {
private:
    static const uint32_t MEMORY_SIZE = 0x10000; // 64KB memory space
    uint8_t* memory;

public:
    Memory();
    ~Memory();
    
    // Memory access methods
    uint8_t load8(uint16_t address) const;
    uint16_t load16(uint16_t address) const;
    void store8(uint16_t address, uint8_t value);
    void store16(uint16_t address, uint16_t value);
    
    // Utility methods
    void clear();
    bool isValidAddress(uint16_t address) const;
};

#endif // MEMORY_H