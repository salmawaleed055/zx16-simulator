#include "z16sim.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <streambuf>


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

// Helper to capture stdout for system call tests
class StdCapture {
private:
    std::ostringstream m_captured;
    std::streambuf* m_old;
public:
    void BeginCapture() {
        m_old = std::cout.rdbuf();
        std::cout.rdbuf(m_captured.rdbuf());
    }
    std::string EndCapture() {
        std::cout.rdbuf(m_old);
        return m_captured.str();
    }
};

// Forward declarations of test functions
void testRType();
void testIType();
void testBType();
void testSType();
void testLType();

int main() {
    std::cout << "Starting Complete Z16 Simulator Tests...\n" << std::endl;

    testRType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testIType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testBType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testSType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    testLType();
    std::cout << "\n-----------------------------------\n" << std::endl;
    // testJType();
    // std::cout << "\n-----------------------------------\n" << std::endl;
    // testUType();
    // std::cout << "\n-----------------------------------\n" << std::endl;
    // testSystemType();

    std::cout << "\nAll tests completed." << std::endl;
    return 0;
}
// --- R-Type Test Cases ---
void testRType() {
    std::cout << "--- R-Type Tests ---" << std::endl;
    z16sim sim;

    std::vector<uint16_t> add_inst = { 0x0440 }; // Correct ADD x1, x2
    createTestBin("test_add.bin", add_inst);
    sim.loadMemoryFromFile("test_add.bin");
    sim.setReg(1, 5);
    sim.setReg(2, 3);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)8, "ADD x1, x2 => x1 = 5 + 3");

    sim = z16sim();
    std::vector<uint16_t> sub_inst = { 0x1440 }; // Correct SUB x1, x2
    createTestBin("test_sub.bin", sub_inst);
    sim.loadMemoryFromFile("test_sub.bin");
    sim.setReg(1, 7);
    sim.setReg(2, 2);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)5, "SUB x1, x2 => x1 = 7 - 2");

    sim = z16sim();
    std::vector<uint16_t> and_inst = { 0x8468 };// Correct AND x1, x2
    createTestBin("test_and.bin", and_inst);
    sim.loadMemoryFromFile("test_and.bin");
    sim.setReg(1, 0xABCD);
    sim.setReg(2, 0x0F0F);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)(0xABCD & 0x0F0F), "AND x1, x2");

    std::cout << "--- R-Type Tests Complete ---" << std::endl;
}

// --- I-Type Test Cases ---
void testIType() {
    std::cout << "--- I-Type Tests ---" << std::endl;
    z16sim sim;

    // ADDI x1, 7 => x1 = x1 + 7
    std::vector<uint16_t> addi_inst = { 0x0E41 };
    createTestBin("test_addi.bin", addi_inst);
    sim.loadMemoryFromFile("test_addi.bin");
    sim.setReg(1, 10);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)(10 + 7), "ADDI x1, 7");

    // SLTI x1, -1 => x1 = (x1 < -1) => 1 if true
    sim = z16sim();
    std::vector<uint16_t> slti_inst = { 0xFE51 };
    createTestBin("test_slti.bin", slti_inst);
    sim.loadMemoryFromFile("test_slti.bin");
    sim.setReg(1, -2);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)1, "SLTI x1, -1");

    // SLLI x1, 4 => x1 = x1 << 4
    sim = z16sim();
    std::vector<uint16_t> slli_inst = { 0x2859 }; // SLLI x1, 4
    createTestBin("test_slli.bin", slli_inst);
    sim.loadMemoryFromFile("test_slli.bin");
    sim.setReg(1, 0x0001);
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)0x0010, "SLLI x1, 4");


    // LI x1, -5 => x1 = -5 (ADDI x1, x0, -5)
    sim = z16sim();
    std::vector<uint16_t> li_inst = { 0xF641 };
    createTestBin("test_li.bin", li_inst);
    sim.loadMemoryFromFile("test_li.bin");
    sim.cycle();
    ASSERT_EQ(sim.getReg(1), (uint16_t)0xFFFB, "LI x1, -5");

    std::cout << "--- I-Type Tests Complete ---" << std::endl;
}


// --- B-Type Test Cases ---
void testBType() {
    std::cout << "--- B-Type Tests ---" << std::endl;

    z16sim sim; // Create a new simulator instance for each test run

    // Test BEQ: x1 == x2, branch taken
    // inst: BEQ x1, x2, +4 (offset to target at PC+4 from current PC)
    // Instruction: [imm4:1][rs2][rs1][fun3][opc] = 0010_010_001_000_010 = 0x2482
    std::vector<uint16_t> insts_beq_taken = {
        0x2482, // BEQ x1, x2, +4 (offset is 4)
        0xAAAA, // NOP (or any other instruction) - This instruction should be skipped
        0x0000  // NOP (Target of branch at PC + 4)
    };
    createTestBin("test_beq_taken.bin", insts_beq_taken);
    sim.loadMemoryFromFile("test_beq_taken.bin");
    sim.setReg(1, 10); // x1 = 10
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BEQ (PC starts at 0, target is 0 + 4 = 4)
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BEQ Taken (x1 == x2)");

    // Test BNE: x1 != x2, branch taken
    sim = z16sim(); // Reset simulator
    // Instruction: [imm4:1][rs2][rs1][fun3][opc] = 0010_010_001_001_010 = 0x24C2
    std::vector<uint16_t> insts_bne_taken = {
        0x24C2, // BNE x1, x2, +4
        0xAAAA, // NOP (skipped)
        0x0000  // NOP (target at PC + 4)
    };
    createTestBin("test_bne_taken.bin", insts_bne_taken);
    sim.loadMemoryFromFile("test_bne_taken.bin");
    sim.setReg(1, 5);  // x1 = 5
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BNE
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BNE Taken (x1 != x2)");

    // Test BNE: x1 == x2, branch not taken
    sim = z16sim(); // Reset simulator
    // Instruction: 0x24C2 (BNE x1, x2, +4)
    std::vector<uint16_t> insts_bne_not_taken = {
        0x24C2, // BNE x1, x2, +4
        0x0000, // NOP (should be executed as branch not taken, PC moves to 0+2=2)
        0xAAAA  // NOP
    };
    createTestBin("test_bne_not_taken.bin", insts_bne_not_taken);
    sim.loadMemoryFromFile("test_bne_not_taken.bin");
    sim.setReg(1, 10); // x1 = 10
    sim.setReg(2, 10); // x2 = 10
    sim.cycle(); // Execute BNE
    ASSERT_EQ(sim.getPC(), (uint16_t)2, "BNE Not Taken (x1 == x2)"); // PC should increment by 2

    // Test BZ: x1 == 0, branch taken
    sim = z16sim(); // Reset simulator
    // Instruction: [imm4:1][rs2][rs1][fun3][opc] = 0010_xxx_001_110_010 = 0x24A2 (rs2 ignored)
    std::vector<uint16_t> insts_bz_taken = {
        0x24A2, // BZ x1, +4
        0xAAAA, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_bz_taken.bin", insts_bz_taken);
    sim.loadMemoryFromFile("test_bz_taken.bin");
    sim.setReg(1, 0); // x1 = 0
    sim.cycle(); // Execute BZ
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BZ Taken (x1 == 0)");

    // Test BLT (signed): x1 < x2, branch taken
    sim = z16sim(); // Reset simulator
    // Instruction: [imm4:1][rs2][rs1][fun3][opc] = 0010_010_001_010_010 = 0x2422
    std::vector<uint16_t> insts_blt_taken = {
        0x2422, // BLT x1, x2, +4
        0xAAAA, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_blt_taken.bin", insts_blt_taken);
    sim.loadMemoryFromFile("test_blt_taken.bin");
    sim.setReg(1, (uint16_t)-5); // x1 = -5 (signed value)
    sim.setReg(2, (uint16_t)10); // x2 = 10 (signed value)
    sim.cycle(); // Execute BLT
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BLT Taken (signed: -5 < 10)");

    // Corrected Test BGEU (unsigned): x1 >= x2, branch taken
    // Instruction: [imm4:1][rs2][rs1][fun3][opc] = 0010_010_001_101_010 = 0x24D2 (funct3=0x5 for BGEU)
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_bgeu_taken = {
        0x24D2, // BGEU x1, x2, +4
        0xAAAA, // NOP (skipped)
        0x0000  // NOP (target)
    };
    createTestBin("test_bgeu_taken.bin", insts_bgeu_taken);
    sim.loadMemoryFromFile("test_bgeu_taken.bin");
    sim.setReg(1, (uint16_t)0xFFFB); // x1 = 0xFFFB (unsigned 65531, signed -5)
    sim.setReg(2, (uint16_t)0x000A); // x2 = 0x000A (unsigned 10, signed 10)
    sim.cycle(); // Execute BGEU (unsigned: 65531 >= 10 is true)
    ASSERT_EQ(sim.getPC(), (uint16_t)4, "BGEU Taken (unsigned: 0xFFFB >= 0x000A)");

    // Add more B-type tests as needed (BLTU, BGE)

    std::cout << "--- B-Type Tests Complete ---" << std::endl;
}

// --- S-Type Test Cases ---
void testSType() {
    std::cout << "--- S-Type Tests ---" << std::endl;

    z16sim sim;

    // Test SB (Store Byte): SB x1, 4(x2)
    // mem[x2 + 4] = x1[7:0]
    // Instruction: [imm3:0][rs2][rs1][fun3][opc] = 0100_001_010_000_011 = 0x4103
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
    // Instruction: [imm3:0][rs2][rs1][fun3][opc] = 1110_001_010_010_011 = 0xE123
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_sw = {
        0xE123 // SW x1, -2(x2)
    };
    createTestBin("test_sw.bin", insts_sw);
    sim.loadMemoryFromFile("test_sw.bin");
    sim.setReg(1, 0xABCD); // x1 = 0xABCD
    sim.setReg(2, 0x1002); // x2 = 0x1002 (base address, target 0x1000)
    sim.cycle(); // Execute SW
    // Expected: memory[0x1000] = 0xCD, memory[0x1001] = 0xAB (little-endian)
    ASSERT_EQ(sim.memory[0x1000], (unsigned char)0xCD, "SW (Store Word) LSB at 0x1000");
    ASSERT_EQ(sim.memory[0x1001], (unsigned char)0xAB, "SW (Store Word) MSB at 0x1001");

    // Test SW (Store Word) - Unaligned Address (should print error/fail and not write)
    sim = z16sim(); // Reset simulator
    // Instruction: [imm3:0][rs2][rs1][fun3][opc] = 1111_001_010_010_011 = 0xF123 (imm=-1)
    std::vector<uint16_t> insts_sw_unaligned = {
        0xF123 // SW x1, -1(x2) (target 0x1001, unaligned)
    };
    createTestBin("test_sw_unaligned.bin", insts_sw_unaligned);
    sim.loadMemoryFromFile("test_sw_unaligned.bin");
    sim.setReg(1, 0xDEAD); // Value to attempt to store
    sim.setReg(2, 0x1002); // Base 0x1002, offset -1 -> target 0x1001 (unaligned)
    sim.cycle(); // Execute SW (this should print an error, not change memory)
    // Verify memory *hasn't* changed from its initial zero state
    ASSERT_EQ(sim.memory[0x1001], (unsigned char)0x00, "SW Unaligned (memory unchanged at 0x1001)");
    ASSERT_EQ(sim.memory[0x1002], (unsigned char)0x00, "SW Unaligned (memory unchanged at 0x1002)");

    // Add more S-type tests as needed

    std::cout << "--- S-Type Tests Complete ---" << std::endl;
}

// --- L-Type Test Cases ---
void testLType() {
    std::cout << "--- L-Type Tests ---" << std::endl;

    z16sim sim;

    // Manually set some memory values for loads
    // These will be reset by z16sim() for each new test, so set them per test if needed.

    // Test LB (Load Byte, sign-extended - positive value): LB x0, 2(x7)
    // x0 = sext(mem[x7 + 2][7:0])
    // Instruction: [imm3:0][rs2][rd][fun3][opc] = 0010_111_000_000_100 = 0x2704 (imm=2)
    std::vector<uint16_t> insts_lb = {
        0x2704 // LB x0, 2(x7)
    };
    createTestBin("test_lb.bin", insts_lb);
    sim.loadMemoryFromFile("test_lb.bin");
    sim.memory[0x2002] = 0xCC; // Set memory for this specific test
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LB
    // Expected: getReg(0) = sext(0xCC) = 0xFFCC (as 0xCC is signed negative)
    ASSERT_EQ(sim.getReg(0), (uint16_t)0xFFCC, "LB (Load Byte, positive data value 0xCC sign-extended) x0 from 0x2002");

    // Test LB (Load Byte, sign-extended - negative value): LB x0, 3(x7)
    sim = z16sim(); // Reset simulator
    // Instruction: [imm3:0][rs2][rd][fun3][opc] = 0011_111_000_000_100 = 0x3704 (imm=3)
    std::vector<uint16_t> insts_lb_neg = {
        0x3704 // LB x0, 3(x7)
    };
    createTestBin("test_lb_neg.bin", insts_lb_neg);
    sim.loadMemoryFromFile("test_lb_neg.bin");
    sim.memory[0x2003] = 0x81; // Set memory for this specific test
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LB
    // Expected: getReg(0) = sext(0x81) = 0xFF81
    ASSERT_EQ(sim.getReg(0), (uint16_t)0xFF81, "LB (Load Byte, negative data value 0x81 sign-extended) x0 from 0x2003");

    // Test LW (Load Word): LW x1, 0(x7)
    // x1 = mem[x7 + 0][15:0] (little-endian)
    // Instruction: [imm3:0][rs2][rd][fun3][opc] = 0000_111_001_010_100 = 0x0724 (imm=0)
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_lw = {
        0x0724 // LW x1, 0(x7)
    };
    createTestBin("test_lw.bin", insts_lw);
    sim.loadMemoryFromFile("test_lw.bin");
    sim.memory[0x2000] = 0xEF; // LSB
    sim.memory[0x2001] = 0xBE; // MSB
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LW
    // Expected: getReg(1) = 0xBEEF
    ASSERT_EQ(sim.getReg(1), (uint16_t)0xBEEF, "LW (Load Word) x1 from 0x2000");

    // Test LBU (Load Byte Unsigned): LBU x0, 3(x7)
    // x0 = zext(mem[x7 + 3][7:0])
    // Instruction: [imm3:0][rs2][rd][fun3][opc] = 0011_111_000_011_100 = 0x3714 (imm=3)
    sim = z16sim(); // Reset simulator
    std::vector<uint16_t> insts_lbu = {
        0x3714 // LBU x0, 3(x7)
    };
    createTestBin("test_lbu.bin", insts_lbu);
    sim.loadMemoryFromFile("test_lbu.bin");
    sim.memory[0x2003] = 0x81; // Set memory for this specific test
    sim.setReg(7, 0x2000); // x7 = 0x2000 (base address)
    sim.cycle(); // Execute LBU
    // Expected: getReg(0) = zext(0x81) = 0x0081
    ASSERT_EQ(sim.getReg(0), (uint16_t)0x0081, "LBU (Load Byte Unsigned) x0 from 0x2003");

    // Test LW (Load Word) - Unaligned Address (should print error/fail and not load)
    sim = z16sim(); // Reset simulator
    // Instruction: [imm3:0][rs2][rd][fun3][opc] = 0001_111_000_010_100 = 0x1724 (imm=1)
    std::vector<uint16_t> insts_lw_unaligned = {
        0x1724 // LW x0, 1(x7) (target 0x1001, unaligned)
    };
    createTestBin("test_lw_unaligned.bin", insts_lw_unaligned);
    sim.loadMemoryFromFile("test_lw_unaligned.bin");
    sim.memory[0x1001] = 0xAA; // Set some values (shouldn't be loaded)
    sim.memory[0x1002] = 0xBB;
    sim.setReg(7, 0x1000); // Base 0x1000, offset 1 -> target 0x1001 (unaligned)
    sim.setReg(0, 0xABCD); // Initial value of rd to check if it changes
    sim.cycle(); // Execute LW (this should print an error, not change regs[0])
    ASSERT_EQ(sim.getReg(0), (uint16_t)0xABCD, "LW Unaligned (register unchanged)");

    // Add more L-type tests as needed

    std::cout << "--- L-Type Tests Complete ---" << std::endl;
}
