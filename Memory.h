#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <stdexcept>
<<<<<<< HEAD
#include <vector>
#include <functional>

class Memory {
public:
    Memory();

    uint8_t load(uint16_t address);
    void store(uint16_t address, uint8_t value);

    uint16_t loadW(uint16_t address);
    void storeW(uint16_t address, uint16_t value);

    void loadFromFile(const char* filename);
    void clear();
    void reset();

    void registerMMIOCallback(uint16_t startAddr, uint16_t endAddr, 
                             std::function<void(uint16_t, uint8_t)> callback);


    static const size_t MEM_SIZE = 65536; // 64kb
    static const uint16_t MMIO_START = 0xF000;
    static const uint16_t MMIO_END = 0xFFFF;
    static const uint16_t GRAPHICS_START = 0xF000;
    static const uint16_t GRAPHICS_END = 0xFA0F;

    bool isValidAddress(uint16_t address) const;
    bool isMMIOAddress(uint16_t address) const;
    void MMIOWrite(uint16_t address, uint8_t value);

private:
        uint8_t memory[MEM_SIZE];

        struct MMIORegion {
            uint16_t startAddress;
            uint16_t endAddress;
            std::function<void(uint16_t, uint8_t)> callback;
        };

        std::vector<MMIORegion> MMIORegions;
  
};



#endif // MEMORY_H
=======

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
>>>>>>> origin/Final_front_end
