#include "z16sim.h" // Include your simulator header
#include <iostream>
#include <vector>
#include <iomanip> // For std::hex, std::setw, std::setfill

// Helper function to create a dummy binary file for testing
void createTestBin(const char* filename, const std::vector<uint16_t>& instructions) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error creating test binary file");
        exit(1);
    }
    for (uint16_t inst : instructions) {
        // Write in little-endian format
        fputc(inst & 0xFF, fp);
        fputc((inst >> 8) & 0xFF, fp);
    }
    fclose(fp);
}

// Macro for simple assertions
#define ASSERT_EQ(actual, expected, test_name) \
    if ((actual) == (expected)) { \
        std::cout << "[PASS] " << test_name << std::endl; \
    } else { \
        std::cout << "[FAIL] " << test_name << ": Expected 0x" << std::hex << (expected) \
                  << ", Got 0x" << std::hex << (actual) << std::dec << std::endl; \
    }

// Forward declarations of test functions
void testBType();
void testSType();
void testLType();


int main() {
    std::cout << "Starting Z16 Simulator Tests (B, S, L Types Only)...\n" << std::endl;

    testBType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testSType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testLType();

    std::cout << "\nAll selected tests completed." << std::endl;
    return 0;
}

// --- B-Type Test Cases ---
void testBType() {
    std::cout << "--- B-Type Tests ---" << std::endl;

    z16sim sim; // Create a new simulator instance for each test run

    // Test BEQ: x1 == x2, branch taken
    // inst: BEQ x1, x2, +4 (offset for next instruction is 2, target is PC+4)
    // Opcode: 010 (2)
    // funct3: 000 (0)
    // rs1: x1 (001)
    // rs2: x2 (010)
    // imm[4:1]: offset = +4.  5-bit offset = 00100 (binary). imm[4:1] = 010.
    // Instruction: 010_010_001_000_010 = 0x2482
    //            [imm4:1][rs2][rs1][fun3][opc]
    // 0x2482: BEQ x1, x2, 0x0004 (if PC is 0x0000)
    // Instructions: BEQ, NOP
    std::vector<uint16_t> insts_beq_taken = {
        0x2482, // BEQ x1, x2, +4 (offset from PC of BEQ to NOP after itself)
        0x0000, // NOP (ADD x0, x0) - Target of branch
        0x0000  // NOP (Should be skipped)
    };
    createTestBin("test_beq_taken.bin", insts_beq_taken);
    sim.loadMemoryFromFile("test_beq_taken.bin");
    sim.setReg(1, 10); // x1 = 10
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BEQ
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BEQ Taken (x1 == x2)");

    // Test BNE: x1 != x2, branch taken
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_bne_taken = {
        0x24C2, // BNE x1, x2, +4. (imm[4:1] = 0010, funct3 = 001, rs1=x1, rs2=x2, opcode=010)
        0x0000, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_bne_taken.bin", insts_bne_taken);
    sim.loadMemoryFromFile("test_bne_taken.bin");
    sim.setReg(1, 5); // x1 = 5
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BNE
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BNE Taken (x1 != x2)");

    // Test BNE: x1 == x2, branch not taken
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_bne_not_taken = {
        0x24C2, // BNE x1, x2, +4
        0x0000, // NOP (should be executed as branch not taken)
        0x0000  // NOP
    };
    createTestBin("test_bne_not_taken.bin", insts_bne_not_taken);
    sim.loadMemoryFromFile("test_bne_not_taken.bin");
    sim.setReg(1, 10); // x1 = 10
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BNE
    ASSERT_EQ(sim.getPC(), (uint16_t)2, "BNE Not Taken (x1 == x2)"); // PC should increment by 2

    // Test BZ: x1 == 0, branch taken
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_bz_taken = {
        0x24A2, // BZ x1, +4. (imm[4:1]=0010, funct3=110, rs1=x1, rs2=ignored, opcode=010)
        0x0000, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_bz_taken.bin", insts_bz_taken);
    sim.loadMemoryFromFile("test_bz_taken.bin");
    sim.setReg(1, 0); // x1 = 0
    sim.cycle(); // Execute BZ
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BZ Taken (x1 == 0)");

    // Test BLT (signed): x1 < x2, branch taken
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_blt_taken = {
        0x2422, // BLT x1, x2, +4 (imm[4:1]=0010, funct3=010, rs1=x1, rs2=x2, opcode=010)
        0x0000, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_blt_taken.bin", insts_blt_taken);
    sim.loadMemoryFromFile("test_blt_taken.bin");
    sim.setReg(1, (uint16_t)-5); // x1 = -5 (signed)
    sim.setReg(2, (uint16_t)10); // x2 = 10 (signed)
    sim.cycle(); // Execute BLT
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BLT Taken (signed: -5 < 10)");

    // Test BGEU (unsigned): x1 >= x2, branch not taken
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_bgeu_not_taken = {
        0x2432, // BGEU x1, x2, +4 (imm[4:1]=0010, funct3=011, rs1=x1, rs2=x2, opcode=010)
        0x0000, // NOP (should execute)
        0x0000  // NOP
    };
    createTestBin("test_bgeu_not_taken.bin", insts_bgeu_not_taken);
    sim.loadMemoryFromFile("test_bgeu_not_taken.bin");
    sim.setReg(1, 5); // x1 = 5 (unsigned)
    sim.setReg(2, 10); // x2 = 10 (unsigned)
    sim.cycle(); // Execute BGEU
    ASSERT_EQ(sim.getPC(), (uint16_t)2, "BGEU Not Taken (unsigned: 5 < 10)");

    std::cout << "--- B-Type Tests Complete ---" << std::endl;
}

// --- S-Type Test Cases ---
void testSType() {
    std::cout << "--- S-Type Tests ---" << std::endl;

    z16sim sim;

    // Test SB (Store Byte): SB x1, 4(x2)
    // mem[x2 + 4] = x1[7:0]
    // opcode: 011 (3)
    // funct3: 000 (0) for SB
    // rs1 (base): x2 (010)
    // rs2 (data): x1 (001)
    // imm[3:0]: 4 (0100)
    // Instruction: 0100_001_010_000_011 = 0x4103
    std::vector<uint16_t> insts_sb = {
        0x4103 // SB x1, 4(x2)
    };
    createTestBin("test_sb.bin", insts_sb);
    sim.loadMemoryFromFile("test_sb.bin");
    sim.setReg(1, 0xABCD); // x1 = 0xABCD
    sim.setReg(2, 0x1000); // x2 = 0x1000 (base address)
    sim.cycle(); // Execute SB
    // Expected: memory[0x1004] should be 0xCD
    ASSERT_EQ(sim.memory[0x1004], (unsigned char)0xCD, "SB (Store Byte) at 0x1004");

    // Test SW (Store Word): SW x1, -2(x2)
    // mem[x2 - 2] = x1[15:0] (little-endian)
    // opcode: 011 (3)
    // funct3: 010 (2) for SW
    // rs1 (base): x2 (010)
    // rs2 (data): x1 (001)
    // imm[3:0]: -2 (1110)
    // Instruction: 1110_001_010_010_011 = 0xE123
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_sw = {
        0xE123 // SW x1, -2(x2)
    };
    createTestBin("test_sw.bin", insts_sw);
    sim.loadMemoryFromFile("test_sw.bin");
    sim.setReg(1, 0xABCD); // x1 = 0xABCD
    sim.setReg(2, 0x1002); // x2 = 0x1002 (base address, target 0x1000)
    sim.cycle(); // Execute SW
    // Expected: memory[0x1000] = 0xCD, memory[0x1001] = 0xAB
    ASSERT_EQ(sim.memory[0x1000], (unsigned char)0xCD, "SW (Store Word) LSB at 0x1000");
    ASSERT_EQ(sim.memory[0x1001], (unsigned char)0xAB, "SW (Store Word) MSB at 0x1001");

    // Test SW (Store Word) - Unaligned Address (should print error/fail and not write)
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_sw_unaligned = {
        // Store at 0x1001 (unaligned)
        // imm[3:0]: -1 (1111)
        // Instruction: 1111_001_010_010_011 = 0xF123
        0xF123 // SW x1, -1(x2)
    };
    createTestBin("test_sw_unaligned.bin", insts_sw_unaligned);
    sim.loadMemoryFromFile("test_sw_unaligned.bin");
    sim.setReg(1, 0xDEAD);
    sim.setReg(2, 0x1002); // Base 0x1002, offset -1 -> target 0x1001 (unaligned)
    sim.cycle(); // Execute SW (this should print an error, not change memory)
    // Verify memory *hasn't* changed (assuming it was 0 initially)
    ASSERT_EQ(sim.memory[0x1001], (unsigned char)0x00, "SW Unaligned (memory unchanged at 0x1001)");
    ASSERT_EQ(sim.memory[0x1002], (unsigned char)0x00, "SW Unaligned (memory unchanged at 0x1002)");


    std::cout << "--- S-Type Tests Complete ---" << std::endl;
}

// --- L-Type Test Cases ---
void testLType() {
    std::cout << "--- L-Type Tests ---" << std::endl;

    z16sim sim;

    // Manually set some memory values for loads
    sim.memory[0x2000] = 0xEF; // LSB for LW
    sim.memory[0x2001] = 0xBE; // MSB for LW
    sim.memory[0x2002] = 0xCC; // For LB/LBU at 0x2002
    sim.memory[0x2003] = 0x81; // For LB signed test (negative value)

    // Test LB (Load Byte, sign-extended): LB x0, 2(x7)
    // x0 = sext(mem[x7 + 2][7:0])
    // opcode: 100 (4)
    // funct3: 000 (0) for LB
    // rs2 (base): x7 (111)
    // rd (dest): x0 (000)
    // imm[3:0]: 2 (0010)
    // Instruction: 0010_111_000_000_100 = 0x2704
    std::vector<uint16_t> insts_lb = {
        0x2704 // LB x0, 2(x7)
    };
    createTestBin("test_lb.bin", insts_lb);
    sim.loadMemoryFromFile("test_lb.bin");
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LB
    // Expected: getReg(0) = sext(0xCC) = 0x00CC
    ASSERT_EQ(sim.getReg(0), (uint16_t)0x00CC, "LB (Load Byte, positive) x0 from 0x2002");

    // Test LB (Load Byte, sign-extended - negative value): LB x0, 3(x7)
    sim = z16sim(); // Reset and setup for specific test
    sim.memory[0x2003] = 0x81; // Load this value for the test
    std::vector<uint16_t> insts_lb_neg = {
        // imm[3:0]: 3 (0011)
        // Instruction: 0011_111_000_000_100 = 0x3704
        0x3704 // LB x0, 3(x7)
    };
    createTestBin("test_lb_neg.bin", insts_lb_neg);
    sim.loadMemoryFromFile("test_lb_neg.bin");
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LB
    // Expected: getReg(0) = sext(0x81) = 0xFF81
    ASSERT_EQ(sim.getReg(0), (uint16_t)0xFF81, "LB (Load Byte, negative) x0 from 0x2003");


    // Test LW (Load Word): LW x1, 0(x7)
    // x1 = mem[x7 + 0][15:0] (little-endian)
    // opcode: 100 (4)
    // funct3: 010 (2) for LW
    // rs2 (base): x7 (111)
    // rd (dest): x1 (001)
    // imm[3:0]: 0 (0000)
    // Instruction: 0000_111_001_010_100 = 0x0724
    sim = z16sim(); // Reset simulator
    sim.memory[0x2000] = 0xEF; // LSB
    sim.memory[0x2001] = 0xBE; // MSB
    std::vector<uint16_t> insts_lw = {
        0x0724 // LW x1, 0(x7)
    };
    createTestBin("test_lw.bin", insts_lw);
    sim.loadMemoryFromFile("test_lw.bin");
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LW
    // Expected: getReg(1) = 0xBEEF
    ASSERT_EQ(sim.getReg(1), (uint16_t)0xBEEF, "LW (Load Word) x1 from 0x2000");

    // Test LBU (Load Byte Unsigned): LBU x0, 3(x7)
    // x0 = zext(mem[x7 + 3][7:0])
    // opcode: 100 (4)
    // funct3: 011 (3) for LBU
    // rs2 (base): x7 (111)
    // rd (dest): x0 (000)
    // imm[3:0]: 3 (0011)
    // Instruction: 0011_111_000_011_100 = 0x3714
    sim = z16sim(); // Reset simulator
    sim.memory[0x2003] = 0x81; // Value for LBU test
    std::vector<uint16_t> insts_lbu = {
        0x3714 // LBU x0, 3(x7)
    };
    createTestBin("test_lbu.bin", insts_lbu);
    sim.loadMemoryFromFile("test_lbu.bin");
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LBU
    // Expected: getReg(0) = zext(0x81) = 0x0081
    ASSERT_EQ(sim.getReg(0), (uint16_t)0x0081, "LBU (Load Byte Unsigned) x0 from 0x2003");


    // Test LW (Load Word) - Unaligned Address (should print error/fail and not load)
    sim = z16sim(); // Reset simulator
    sim.memory[0x1001] = 0xAA; // Set some value
    sim.memory[0x1002] = 0xBB; // Set some value
    sim.setReg(0, 0xDEAD); // Initial value to check if it changes
    std::vector<uint16_t> insts_lw_unaligned = {
        // imm[3:0]: 1 (0001)
        // Instruction: 0001_111_000_010_100 = 0x1724
        0x1724 // LW x0, 1(x7) (target 0x1001)
    };
    createTestBin("test_lw_unaligned.bin", insts_lw_unaligned);
    sim.loadMemoryFromFile("test_lw_unaligned.bin");
    sim.setReg(7, 0x1000); // Base 0x1000, offset 1 -> target 0x1001 (unaligned)
    sim.setReg(0, 0xABCD); // Initial value of rd
    sim.cycle(); // Execute LW (this should print an error, not change regs[0])
    ASSERT_EQ(sim.getReg(0), (uint16_t)0xABCD, "LW Unaligned (register unchanged)");


    std::cout << "--- L-Type Tests Complete ---" << std::endl;
}