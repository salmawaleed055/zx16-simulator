#include "z16sim.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>

// Load binary file into simulator memory and report size
size_t loadBinary(z16sim& sim, const std::string& filename) {
    sim.loadMemoryFromFile(filename.c_str());

    std::ifstream f(filename, std::ios::binary | std::ios::ate);
    return static_cast<size_t>(f.tellg());
}

// Normalize line endings (\r\n vs \n)
std::string normalizeLine(const std::string& line) {
    std::string result = line;
    if (!result.empty() && result.back() == '\r') result.pop_back();
    return result;
}

// Compare output vs expected file
bool compareFiles(const std::string& f1, const std::string& f2) {
    std::ifstream file1(f1), file2(f2);
    std::string line1, line2;
    int lineNum = 1;

    while (std::getline(file1, line1) && std::getline(file2, line2)) {
        if (normalizeLine(line1) != normalizeLine(line2)) {
            std::cout << "[DIFF] Line " << lineNum << " differs:\n";
            std::cout << "  File1: " << line1 << "\n";
            std::cout << "  File2: " << line2 << "\n";
            return false;
        }
        ++lineNum;
    }

    if (std::getline(file1, line1) || std::getline(file2, line2)) {
        std::cout << "[DIFF] One file has extra lines.\n";
        return false;
    }

    return true;
}

// Assertion macro
#define ASSERT_EQ(actual, expected, test_name) \
if ((actual) == (expected)) { \
    std::cout << "[PASS] " << test_name << std::endl; \
} else { \
    std::cout << "[FAIL] " << test_name << ": Expected 0x" << std::hex << (expected) \
              << ", Got 0x" << std::hex << (actual) << std::dec << std::endl; \
}

void test_TC_ZX16_01() {
    std::cout << "Running Test Case: TC-ZX16-01_ADD" << std::endl;

    z16sim sim;
    const std::string binFile = "D:/Assembly/Assembly Project/zx16-simulator/Tests/TC-ZX16-01_ADD.bin";
    const std::string outputFile = "D:/Assembly/Assembly Project/zx16-simulator/Tests/TC-ZX16-01_ADD.output";
    const std::string expectedFile = "D:/Assembly/Assembly Project/zx16-simulator/Tests/TC-ZX16-01_ADD.expected";

    size_t loadedBytes = loadBinary(sim, binFile);

    std::ofstream out(outputFile);
    out << "Loaded " << loadedBytes << " bytes into memory\n";

    // Simulate and disassemble instructions
    for (int i = 0; i < 10; ++i) {
        uint16_t pc = sim.getPC();
        uint16_t inst = sim.memory[pc] | (sim.memory[pc + 1] << 8);

        // Disassemble current instruction
        char disasm[64];
        std::snprintf(disasm, sizeof(disasm), "0x%04X: %04X    ", pc, inst);
        sim.disassemble(inst, pc, disasm + std::strlen(disasm), sizeof(disasm) - std::strlen(disasm));
        out << disasm << "\n";

        // Run instruction
        sim.cycle();

        // Stop on ECALL (0xFFC7)
        if (inst == 0xFFC7) break;
    }

    // Check result: x1 should be 8
    ASSERT_EQ(sim.getReg(1), 8, "ADD x1 = x1 + x2");

    // Dump final state
    out << "\n--- Final State ---\n";
    for (int i = 0; i < z16sim::NUM_REGS; ++i) {
        out << z16sim::regNames[i] << ": 0x"
            << std::hex << std::setw(4) << std::setfill('0') << sim.getReg(i) << "\n";
    }
    out << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << sim.getPC() << "\n";
    out << "---------------------\n\nSimulation finished.\n";
    out.close();

    // Compare with expected file
    if (compareFiles(outputFile, expectedFile)) {
        std::cout << "[PASS] Output matches expected file" << std::endl;
    } else {
        std::cout << "[FAIL] Output does not match expected file" << std::endl;
    }
}

int main() {
    std::cout << "===== ZX16 Simulator Test Runner =====\n" << std::endl;
    test_TC_ZX16_01();
    std::cout << "\nAll tests completed." << std::endl;
    return 0;
}
