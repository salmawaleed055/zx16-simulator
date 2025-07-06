#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint> // For uint16_t
#include <string>  // For std::string


// Helper function to create a binary file from a vector of 16-bit instructions
void createBinFile(const std::string& filename, const std::vector<uint16_t>& instructions) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error creating binary file: " << filename << std::endl;
        exit(1);
    }
    for (uint16_t inst : instructions) {
        // Write in little-endian format (low byte first, then high byte)
        ofs.put(inst & 0xFF);         // Lower 8 bits
        ofs.put((inst >> 8) & 0xFF);  // Upper 8 bits
    }
    ofs.close();
    std::cout << "Generated: " << filename << std::endl;
}

int main() {
    std::cout << "Generating all Z16 test binary files..." << std::endl;

    // NOP instruction (ADD x0, x0) often used as filler or skipped instruction
    const uint16_t NOP = 0x0000;

    std::vector<uint16_t> graphics_insts = {
        0x006E, // LUI x1, 0x1F4
        0x0061, // ORI x1, x1, 0
        0xC041, // ORI x2, x0, -32 (0xE0)
        0x1223, // SB x2, 1(x1)
        0xFFC7  // ECALL 0x3FF
    };
    createBinFile("tests_new/graphics.bin", graphics_insts);

    std::cout << "\nAll binary test files generated." << std::endl;
    return 0;
}