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

    std::vector<uint16_t> tc_03_and_insts = {
        // li x1, 7 (imm=7, rd=x1(1), funct3=7, opcode=1) => (7<<9)|(1<<6)|(7<<3)|1 = 0x0E79
        0x0E79,
        // li x2, 2 (imm=2, rd=x2(2), funct3=7, opcode=1) => (2<<9)|(2<<6)|(7<<3)|1 = 0x04B9
        0x04B9,
        // and x1, x1, x2 (funct4=8, rs2=x2(2), rd_rs1=x1(1), funct3=5, opcode=0) => (8<<12)|(2<<9)|(1<<6)|(5<<3)|0 = 0x8468
        0x8468,
        // ecall 0x3FF
        0xFFC7
    };
    createBinFile("tests/TC-ZX16-03_AND.bin", tc_03_and_insts);

    std::cout << "\nAll binary test files generated." << std::endl;
    return 0;
}