#include "Memory.h"
#include <iostream>
#include <fstream>
#include <cstring>

Memory::Memory() {
    clear();
}

uint8_t Memory::load(uint16_t address) {
    if (!isValidAddress(address)) {
        std::cerr << "Invalid Address 0x"  << std::hex << address << std::dec << std::endl;
        return 0;
    }

    return memory[address];
}


void Memory::store(uint16_t address, uint8_t value) {
    if (!isValidAddress(address)) {
        std::cerr << "Invalid Address 0x"  << std::hex << address << std::dec << std::endl;
        return;
    }

    memory[address] = value;

    if (isMMIOAddress(address))
        MMIOWrite(address, value);
}


uint16_t Memory::loadW(uint16_t address) {
    if (!isValidAddress(address) || !isValidAddress(address + 1)) {
        std::cerr << "Invalid address 0x" 
                    << std::hex << address << std::dec << std::endl;
        return 0;
    }

     uint16_t low = memory[address];
    uint16_t high = memory[address + 1];
    return low | (high << 8); // little endian
}


void Memory::storeW(uint16_t address, uint16_t value) {
    if (!isValidAddress(address) || !isValidAddress(address + 1)) {
    std::cerr << "Invalid address 0x" 
                << std::hex << address << std::dec << std::endl;
    return;
    }
 
    memory[address] = value & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF; // little endian

    if (isMMIOAddress(address)) {
        MMIOWrite(address, memory[address]);
    }
    if (isMMIOAddress(address + 1)) {
        MMIOWrite(address + 1, memory[address + 1]);
    }
}


void Memory::loadFromFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }
    
    file.read(reinterpret_cast<char*>(memory), MEM_SIZE);
    if (file.bad()) {
        std::cerr << "Failed to read file: " << filename << std::endl;
    }
    
    file.close();    
}


void Memory::clear() {
    memset(memory, 0, MEM_SIZE); // clear data in memory
}

void Memory::reset() { //clear and reset memory regions
    clear();
    MMIORegions.clear();
}


void Memory::registerMMIOCallback(uint16_t startAddr, uint16_t endAddr, 
                                 std::function<void(uint16_t, uint8_t)> callback) {
    MMIORegion region;
    region.startAddress = startAddr;
    region.endAddress = endAddr;
    region.callback = callback;
    MMIORegions.push_back(region);
}


bool Memory::isValidAddress(uint16_t address) const {
    return address < MEM_SIZE;
}


bool Memory::isMMIOAddress(uint16_t address) const {
    return (address >= MMIO_START && address <= MMIO_END);
}

void Memory::MMIOWrite(uint16_t address, uint8_t value) {
    for (const auto& region : MMIORegions) 
        if (address >= region.startAddress && address <= region.endAddress) 
            region.callback(address, value);
        
}