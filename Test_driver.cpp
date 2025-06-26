#include "z16sim.h"  // Your ZX16 simulator class
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

// Load binary file into simulator memory
void loadBinary(z16sim& sim, const std::string& filename) {
    sim.loadMemoryFromFile(filename.c_str());
}

// Compare two output files (actual vs expected)
bool compareFiles(const std::string& f1, const std::string& f2) {
    std::ifstream file1(f1), file2(f2);
    std::string s1, s2;
    file1 >> s1;
    file2 >> s2;
    return s1 == s2;
}

// Assertion macro
#define ASSERT_EQ(actual, expected, test_name) \
if ((actual) == (expected)) { \
std::cout << "[PASS] " << test_name << std::endl; \
} else { \
std::cout << "[FAIL] " << test_name << ": Expected 0x" << std::hex << (expected) \
<< ", Got 0x" << std::hex << (actual) << std::dec << std::endl; \
}

// Test Case: ADD x3 = x1 + x2
void test_TC_ZX16_01() {
    std::cout << "Running Test Case: TC-ZX16-01" << std::endl;

    z16sim sim;
    loadBinary(sim, "tests/TC-ZX16-01.bin");

    // Set registers x1 = 5, x2 = 10
    sim.setReg(1, 5);
    sim.setReg(2, 10);

    // Run 1 cycle: ADD x3, x1, x2
    sim.cycle();

    // Check result: x3 should be 15
    uint16_t result = sim.getReg(3);
    ASSERT_EQ(result, 15, "ADD x3 = x1 + x2");

    // Write result to output file
    std::ofstream out("tests/TC-ZX16-01.output");
    out << std::hex << std::setfill('0') << std::setw(4) << result;
    out.close();

    // Compare with expected file
    if (compareFiles("tests/TC-ZX16-01.output", "tests/TC-ZX16-01.expected")) {
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
