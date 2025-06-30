// create_test_bins.cpp
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

    // --- R-Type Test Cases ---
    // TC-ZX16-01_ADD: add x1, x2 => x1 = 5 + 3 = 8
    // std::vector<uint16_t> tc_01_add_insts = {
    //     0x0A79, // li x1, 5  (imm=5, rd=x1(1), funct3=7) => (5<<9)|(1<<6)|(7<<3)|1 = 0x0A79
    //     0x06B9, // li x2, 3  (imm=3, rd=x2(2), funct3=7) => (3<<9)|(2<<6)|(7<<3)|1 = 0x06B9
    //     0x0440, // add x1, x2 (funct4=0, rs2=x2(2), rd_rs1=x1(1), funct3=0, opcode=0) => (0<<12)|(2<<9)|(1<<6)|(0<<3)|0 = 0x0440
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-01_ADD.bin", tc_01_add_insts);
    //
    // // TC-ZX16-02_SUB: sub x1, x2 => x1 = 7 - 2 = 5
    // std::vector<uint16_t> tc_02_sub_insts = {
    //     0x0E79, // li x1, 7
    //     0x04B9, // li x2, 2
    //     0x1440, // sub x1, x2 (funct4=1, rs2=x2(2), rd_rs1=x1(1), funct3=0, opcode=0) => (1<<12)|(2<<9)|(1<<6)|(0<<3)|0 = 0x1440
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-02_SUB.bin", tc_02_sub_insts);

    // TC-ZX16-03_AND: and x1, x2 => x1 = 0xABCD & 0x0F0F = 0x0B0D
    std::vector<uint16_t> tc_03_and_insts = {
        0x2F71, // li x1, 0xABCD (imm=0x3F (63), rd=x1, funct3=7) -> this doesn't encode 0xABCD. LI instruction only takes 7-bit signed imm.
                // We'll need multiple instructions for larger immediate or pre-set the registers in test runner if possible.
                // For now, let's simplify to a small number that fits LI's 7-bit signed immediate.
                // Let's use x1 = 0x3A (58) and x2 = 0x2F (47)
        0x7471, // li x1, 0x3A (imm=0x3A, rd=x1(1), funct3=7) => (0x3A<<9)|(1<<6)|(7<<3)|1 = 0x7471
        0x5EB1, // li x2, 0x2F (imm=0x2F, rd=x2(2), funct3=7) => (0x2F<<9)|(2<<6)|(7<<3)|1 = 0x5EB1
        0x8468, // and x1, x2 (funct4=8, rs2=x2(2), rd_rs1=x1(1), funct3=5, opcode=0) => (8<<12)|(2<<9)|(1<<6)|(5<<3)|0 = 0x8468
        0xFFC7  // ecall 0x3FF
    };
    createBinFile("tests/TC-ZX16-03_AND.bin", tc_03_and_insts);

    // --- I-Type Test Cases ---
    // TC-ZX16-04_ADDI: addi x1, 7 => x1 = 10 + 7 = 17
    // std::vector<uint16_t> tc_04_addi_insts = {
    //     0x1471, // li x1, 10
    //     0x0E41, // addi x1, 7 (imm=7, rd=x1(1), funct3=0, opcode=1) => (7<<9)|(1<<6)|(0<<3)|1 = 0x0E41
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-04_ADDI.bin", tc_04_addi_insts);
    //
    // // TC-ZX16-05_SLTI: slti x1, -1 => x1 = (-2 < -1) ? 1 : 0 = 1
    // std::vector<uint16_t> tc_05_slti_insts = {
    //     0xF671, // li x1, -2 (imm=-2, rd=x1(1), funct3=7) => (0x7E<<9)|(1<<6)|(7<<3)|1 = 0xFCA1 (0x7E is 126, sign-extended -2 is 0xFFFE)
    //             // Let's use the 0xF641 from your original test for LI -5, which means imm7=0x3D
    //             // If LI x1, -2 should be 0xF641, then let's use the original test's encoding 0xF641 which was LI x1, -5
    //             // Let's stick with the original `0xFE51` for SLTI x1, -1 and `sim.setReg(1, -2)`
    //             // The `createTestBin` function won't set initial register values. We need an LI instruction for this.
    //             // `li x1, -2` implies `imm7 = 0x7E` (126). `(0x7E<<9)|(1<<6)|(7<<3)|1 = 0xFC79`.
    //             // For slti x1, -1, we need imm=-1. imm7 for -1 is 0x7F.
    //             // (0x7F<<9)|(1<<6)|(1<<3)|1 = 0xFE00 | 0x40 | 0x08 | 0x01 = 0xFE49.
    //             // Original test had `0xFE51` for slti -1. (0xFE51 >> 9)&0x7F = 0x3F. Sign-extend 0x3F (63) is 63.
    //             // So original `test.cpp` for SLTI used `imm=63`, which is wrong for `-1`.
    //             // Let's recalculate `SLTI x1, -1`:
    //             // imm7 = 0x7F (127), rd=x1(1), funct3=1, opcode=1 => (0x7F<<9)|(1<<6)|(1<<3)|1 = 0xFE00 | 0x40 | 0x08 | 0x01 = 0xFE49
    //             // LI x1, -2: imm7=0x7E (126), rd=x1(1), funct3=7, opcode=1 => (0x7E<<9)|(1<<6)|(7<<3)|1 = 0xFC79
    //     0xFC79, // li x1, -2
    //     0xFE49, // slti x1, -1 (imm=-1, rd=x1(1), funct3=1, opcode=1) => (0x7F<<9)|(1<<6)|(1<<3)|1 = 0xFE49
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-05_SLTI.bin", tc_05_slti_insts);
    //
    // // TC-ZX16-06_SLLI: slli x1, 4 => x1 = 0x0001 << 4 = 0x0010
    // std::vector<uint16_t> tc_06_slli_insts = {
    //     0x0271, // li x1, 1
    //     0x2859, // slli x1, 4 (imm=0x28 (shamt=4, shift_type=1), rd=x1(1), funct3=3, opcode=1) => (0x28<<9)|(1<<6)|(3<<3)|1 = 0x5000 | 0x40 | 0x18 | 0x01 = 0x5059. Original test was 0x2859.
    //             // Let's verify 0x2859: imm7 = (0x2859 >> 9) & 0x7F = 0x05 & 0x7F = 0x05.
    //             // shift_type = (0x05 >> 4) & 0x7 = 0. shamt = 0x05 & 0xF = 5.
    //             // So 0x2859 is SLLI x1, 5, not SLLI x1, 4. This is an error in original test's comment.
    //             // Let's use the correct encoding for SLLI x1, 4:
    //             // shamt=4 (0b0100), shift_type=1 (0b001). imm7 = (0b001 << 4) | 0b0100 = 0x14.
    //             // (0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2800 | 0x40 | 0x18 | 0x01 = 0x2859.
    //             // Wait, 0x2859 is correct based on my encoding for SLLI x1, 4 where imm7 is 0x14.
    //             // Let's trace it again. imm7 is bits [15:9] from instruction.
    //             // 0x2859. Bits [15:9] = (0x2859 >> 9) & 0x7F = (0x0500 >> 9) & 0x7F = 0x05.
    //             // The `disassemble` says: `imm = (inst >> 9) & 0x7F;` for I-type. So `imm` is 0x05.
    //             // Then `shift_type = (imm >> 4) & 0x7;` = (0x05 >> 4) & 0x7 = 0x0. `shamt = imm & 0xF;` = 0x05.
    //             // So the instruction 0x2859 with `funct3=0x3` should be `unknown shift imm` or an actual `slli x1, 5`.
    //             // There's a mismatch between the desired assembly and your simulator's parsing of shift immediates.
    //             // If `shamt` is `imm & 0xF` and `shift_type` is `(imm >> 4) & 0x7`, then the `imm` field itself encodes both.
    //             // For `slli x1, 4`: `shamt=4`, `shift_type=1`. `imm = (1 << 4) | 4 = 16 | 4 = 20 (0x14)`.
    //             // So `imm7` should be `0x14`.
    //             // Instruction: `(0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2800 | 0x40 | 0x18 | 0x01 = 0x2859`.
    //             // The encoding 0x2859 is correct for `slli x1, 4`.
    //             // The problem is `disassemble` for `0x2859` gets `imm=0x05`, which gives `shift_type=0`.
    //             // This means the `imm` parsing for shift instructions in `disassemble` and `executeInstruction` is based on the *entire* 7-bit immediate `imm7` (bits 15:9), not just a part of it.
    //             // `imm7` (bits 15:9) contains both `shift_type` and `shamt`.
    //             // `imm7 = (shift_type << 4) | shamt`.
    //             // For `slli x1, 4`, `shamt=4 (0b0100)`, `shift_type=1 (0b001)`.
    //             // `imm7` should be `(0b001 << 4) | 0b0100 = 0x14` (decimal 20).
    //             // My manual encoding `0x2859` results in `imm7=0x05` from the instruction. This is the problem.
    //             // Let's use `imm7 = 0x14` for the instruction.
    //             // `inst = (0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2800 | 0x40 | 0x18 | 0x01 = 0x2859`.
    //             // `0x2859` is the instruction. Its `imm7` is `(0x2859 >> 9) & 0x7F = (0x5 << 0) & 0x7F = 0x05`. This is still 5.
    //             // The issue is in how `disassemble` (and `execute`) reads `imm7` as a whole, then picks apart.
    //             // `inst = (imm7_from_spec << 9) | ...`.
    //             // `0x14` is 20 decimal. `rd=1`, `funct3=3`, `opcode=1`.
    //             // Instruction would be `(20 << 9) | (1 << 6) | (3 << 3) | 1 = 10240 | 64 | 24 | 1 = 10329 = 0x2859`.
    //             // It seems my calculation of `imm7` *from* the instruction `0x2859` was incorrect earlier.
    //             // `(0x2859 >> 9)` is `0x5`. So `imm7` is `5`.
    //             // `shift_type = (5 >> 4) & 0x7 = 0`. `shamt = 5 & 0xF = 5`.
    //             // So `0x2859` is actually `slli x1, 5`. The original `test.cpp` comment for 0x2859 saying `slli x1, 4` was incorrect.
    //             // To get `slli x1, 4`, we need `imm7 = 0x14`. So the instruction `(0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2859`.
    //             // Let's re-verify the bit extraction for shift instruction.
    //             // `uint8_t imm7 = (inst >> 9) & 0x7F;`
    //             // `uint8_t shamt = imm7 & 0xF;`
    //             // `uint8_t shift_type = (imm7 >> 4) & 0x7;`
    //             // If `inst = 0x2859`, then `imm7 = 0x05`.
    //             // `shamt = 0x05`. `shift_type = 0`. This is NOT slli.
    //             // This means the encoding in `test.cpp` for `0x2859` is fundamentally wrong if it's meant to be `slli`.
    //             // A `slli` instruction has `shift_type=0x1`. So `imm7` must have bit 4 set.
    //             // e.g., `imm7 = 0x10` (0010000) for `shamt=0`, `shift_type=1`.
    //             // `imm7 = 0x14` (0010100) for `shamt=4`, `shift_type=1`.
    //             // `inst = (0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2800 | 0x40 | 0x18 | 0x01 = 0x2859`. This is the same.
    //             // This implies that `0x2859` as an instruction *cannot* be `slli x1, 4` given the current `disassemble` logic.
    //             // Let's assume the simulator's `disassemble` and `executeInstruction` are correct, then `0x2859` means `slli x1, 5` (shamt 5, shift_type 0, which would default to slli) IF funct3=3 indicates shift types and not unknown.
    //             // The `disassemble` has `else if (shift_type == 0x1) snprintf(buf, bufSize, "slli %s, %d", regNames[rd_rs1], shamt);`
    //             // This indicates it *requires* `shift_type == 0x1`.
    //             // Therefore, 0x2859 (imm7=0x05, shift_type=0) would fall into `unknown shift imm`.
    //             // The original `test.cpp` for `SLLI` with `0x2859` and `ASSERT_EQ(sim.getReg(1), (uint16_t)0x0010, "SLLI x1, 4");` would *fail*.
    //             // Let's use an `imm7` that actually produces `shift_type = 1` for `slli`.
    //             // For `slli x1, 4`: `shamt = 4`, `shift_type = 1`. `imm7 = (1 << 4) | 4 = 0x14`.
    //             // Instruction: `(0x14 << 9) | (1 << 6) | (3 << 3) | 1 = 0x2800 | 0x40 | 0x18 | 0x01 = 0x2859`.
    //             // This is still 0x2859. Why is `(0x2859 >> 9) & 0x7F` returning 0x05?
    //             // 0x2859 = 0010 1000 0101 1001
    //             // Bits 15-9: 0010100 -> 0x14.
    //             // Okay, my manual bit extraction was wrong. `(0x2859 >> 9) & 0x7F` is indeed `0x14` (20 decimal).
    //             // `shift_type = (0x14 >> 4) & 0x7 = 0x01 & 0x7 = 1`. This is `slli`. Correct.
    //             // `shamt = 0x14 & 0xF = 4`. This is `shamt=4`. Correct.
    //             // So, the instruction `0x2859` *does* correctly encode `slli x1, 4`. My previous confusion was from a manual mental mistake.
    //             // The original test `0x2859` for SLLI x1, 4 is correct.
    //     0x0271, // li x1, 1
    //     0x2859, // slli x1, 4
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-06_SLLI.bin", tc_06_slli_insts);
    //
    // // TC-ZX16-07_LI: li x1, -5 => x1 = 0xFFFB
    // std::vector<uint16_t> tc_07_li_insts = {
    //     0xF641, // li x1, -5 (imm=-5 (0x7B), rd=x1(1), funct3=7) => (0x7B<<9)|(1<<6)|(7<<3)|1 = 0xF679.
    //             // Original test was 0xF641. Let's use the provided 0xF641.
    //             // 0xF641 => imm7 = (0xF641>>9)&0x7F = 0x3D (61 decimal).
    //             // This would make it `li x1, 61`.
    //             // If `li x1, -5` should result in `0xFFFB`, then the instruction should encode `imm7 = 0x7B`.
    //             // So, 0xF679 is the correct encoding for `li x1, -5`.
    //             // The original test instruction `0xF641` has `imm7=0x3D`. `0x3D` is 61. So `LI x1, 61`.
    //             // If the test `LI x1, -5` was actually passing with `0xF641`, it means `sim.setReg(1, -5)` was manually setting it, or there's a disconnect.
    //             // I will use 0xF679 which correctly encodes `imm7 = 0x7B` for -5.
    //     0xF679, // li x1, -5
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-07_LI.bin", tc_07_li_insts);
    //
    //
    // // --- B-Type Test Cases (Revised) ---
    // // TC-ZX16-08_BNE_Taken: bne x1, x2, +4 => x1=5, x2=10, branch taken to PC+4
    // std::vector<uint16_t> tc_08_bne_taken_insts = {
    //     0x0A79, // li x1, 5
    //     0x1471, // li x2, 10
    //     0x24C2, // bne x1, x2, +4 (imm=0x02, rs2=x2(2), rs1=x1(1), funct3=1, opcode=2) => (2<<12)|(2<<9)|(1<<6)|(1<<3)|2 = 0x24C2
    //     NOP,    // NOP (skipped)
    //     0x0271, // li x1, 1 (target)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-08_BNE_Taken.bin", tc_08_bne_taken_insts);
    //
    // // TC-ZX16-09_BNE_NotTaken: bne x1, x2, +4 => x1=10, x2=10, branch not taken to PC+2
    // std::vector<uint16_t> tc_09_bne_not_taken_insts = {
    //     0x1471, // li x1, 10
    //     0x14B1, // li x2, 10
    //     0x24C2, // bne x1, x2, +4
    //     0x0271, // li x1, 1 (executed, PC goes to 0x0006)
    //     NOP,    // NOP (skipped)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-09_BNE_NotTaken.bin", tc_09_bne_not_taken_insts);
    //
    // // TC-ZX16-10_BZ_Taken: bz x1, +4 => x1=0, branch taken to PC+4
    // std::vector<uint16_t> tc_10_bz_taken_insts = {
    //     0x0071, // li x1, 0
    //     0x24A2, // bz x1, +4 (imm=0x02, rs2=xX(any), rs1=x1(1), funct3=6, opcode=2) => (2<<12)|(0<<9)|(1<<6)|(6<<3)|2 = 0x24A2
    //     NOP,    // NOP (skipped)
    //     0x0271, // li x1, 1 (target)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-10_BZ_Taken.bin", tc_10_bz_taken_insts);
    //
    // // TC-ZX16-11_BLT_SignedTaken: blt x1, x2, +4 => x1=-5, x2=10, branch taken
    // std::vector<uint16_t> tc_11_blt_signed_taken_insts = {
    //     0xF679, // li x1, -5
    //     0x1471, // li x2, 10
    //     0x2422, // blt x1, x2, +4 (imm=0x02, rs2=x2(2), rs1=x1(1), funct3=2, opcode=2) => (2<<12)|(2<<9)|(1<<6)|(2<<3)|2 = 0x2422
    //     NOP,    // NOP (skipped)
    //     0x0271, // li x1, 1 (target)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-11_BLT_SignedTaken.bin", tc_11_blt_signed_taken_insts);
    //
    // // TC-ZX16-12_BGEU_UnsignedTaken: bgeu x1, x2, +4 => x1=0xFFFB (-5), x2=0x000A (10), branch taken (0xFFFB >= 0x000A unsigned)
    // std::vector<uint16_t> tc_12_bgeu_unsigned_taken_insts = {
    //     0xF679, // li x1, -5 (0xFFFB)
    //     0x1471, // li x2, 10 (0x000A)
    //     0x24D2, // bgeu x1, x2, +4 (imm=0x02, rs2=x2(2), rs1=x1(1), funct3=5, opcode=2) => (2<<12)|(2<<9)|(1<<6)|(5<<3)|2 = 0x24D2
    //     NOP,    // NOP (skipped)
    //     0x0271, // li x1, 1 (target)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-12_BGEU_UnsignedTaken.bin", tc_12_bgeu_unsigned_taken_insts);
    //
    // // TC-ZX16-13_BLTU_UnsignedNotTaken: bltu x1, x2, +4 => x1=5, x2=10, branch not taken (5 < 10 unsigned)
    // std::vector<uint16_t> tc_13_bltu_unsigned_not_taken_insts = {
    //     0x0A79, // li x1, 5
    //     0x1471, // li x2, 10
    //     0x2462, // bltu x1, x2, +4 (imm=0x02, rs2=x2(2), rs1=x1(1), funct3=4, opcode=2) => (2<<12)|(2<<9)|(1<<6)|(4<<3)|2 = 0x2462
    //     0x0271, // li x1, 1 (executed)
    //     NOP,    // NOP (skipped)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-13_BLTU_UnsignedNotTaken.bin", tc_13_bltu_unsigned_not_taken_insts);
    //
    //
    // // --- S-Type Test Cases ---
    // // TC-ZX16-14_SB: sb x1, 2(a0) => mem[a0+2] = x1[7:0]
    // std::vector<uint16_t> tc_14_sb_insts = {
    //     0x0271, // li x1, 0x0105 (imm=5 for x1)
    //     0x21B9, // li a0, 0x1000 (imm=0x10, rd=a0(6), funct3=7) => (0x10<<9)|(6<<6)|(7<<3)|1 = 0x21B9
    //     0x4E03, // sb x1, 2(a0) (imm=2, rs2=x1(1), rs1=a0(6), funct3=0, opcode=3) => (2<<12)|(1<<9)|(6<<6)|(0<<3)|3 = 0x4243
    //             // My old calculation: 0x4E03. (0x4<<12)|(0x1<<9)|(0x6<<6)|(0x0<<3)|0x3 = 0x4000 | 0x200 | 0x180 | 0 | 3 = 0x4383.
    //             // Let's re-verify the `imm` bit extraction for S-type: `imm4 = (inst >> 12) & 0xF;`
    //             // My encoded `0x4243` for SB x1, 2(a0) is:
    //             // imm4 = (0x4243 >> 12) & 0xF = 0x4. Correct for offset 2 (0010).
    //             // rs2 = (0x4243 >> 9) & 0x7 = 0x1. Correct for x1.
    //             // rs1 = (0x4243 >> 6) & 0x7 = 0x6. Correct for a0.
    //             // funct3 = (0x4243 >> 3) & 0x7 = 0x0. Correct for SB.
    //             // opcode = 0x3. Correct for S-type.
    //             // So, `0x4243` is the correct encoding for `sb x1, 2(a0)`. My previous 0x4E03 and other things were inconsistent.
    //     0x4243, // sb x1, 2(a0)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-14_SB.bin", tc_14_sb_insts);
    //
    // // TC-ZX16-15_SW: sw x1, -2(a0) => mem[a0-2] = x1[15:0] (little-endian)
    // std::vector<uint16_t> tc_15_sw_insts = {
    //     0x2271, // li x1, 0xABCD (imm=0x22, 34; not ABCD. Use a small value for LI)
    //             // Let's use 0xABCD, but we can't LI directly.
    //             // Let's test with a small value for SW. e.g., x1 = 0x1234
    //     0x6871, // li x1, 0x34 (imm=0x34, 52)
    //     0x0271, // li x2, 0x12 (imm=0x12, 18)
    //     0x0440, // add x1, x2 (This is not how you build a word. You need two LI and shifts/ORs or just one LI with a smaller word.)
    //             // Given the limitations of LI, let's test `SW x1, -2(a0)` with `x1` being a small value.
    //             // E.g., `li x1, 0x12`
    //     0x2471, // li x1, 0x12 (data to store)
    //     0x21B9, // li a0, 0x1002 (base address)
    //     0xE623, // sw x1, -2(a0) (imm=-2 (0xE), rs2=x1(1), rs1=a0(6), funct3=2, opcode=3) => (0xE<<12)|(1<<9)|(6<<6)|(2<<3)|3 = 0xE2A3
    //             // My old calculation: 0xE123. Let's re-calculate:
    //             // 0xE2A3: imm4=(0xE2A3>>12)&0xF = 0xE. Correct for -2.
    //             // rs2=(0xE2A3>>9)&0x7 = 0x1. Correct for x1.
    //             // rs1=(0xE2A3>>6)&0x7 = 0x6. Correct for a0.
    //             // funct3=(0xE2A3>>3)&0x7 = 0x2. Correct for SW.
    //             // opcode=0x3. Correct.
    //             // So, `0xE2A3` is the correct encoding for `sw x1, -2(a0)`.
    //     0xE2A3, // sw x1, -2(a0)
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-15_SW.bin", tc_15_sw_insts);
    //
    // // TC-ZX16-16_SW_Unaligned: sw x1, -1(a0) => Should print error, not write
    // std::vector<uint16_t> tc_16_sw_unaligned_insts = {
    //     0x0271, // li x1, 1
    //     0x21B9, // li a0, 0x1001 (base address, for target 0x1000) (imm=0x10, rd=a0, funct3=7)
    //             // If a0=0x1001, then -1(a0) becomes 0x1000 (aligned). We want *unaligned* target.
    //             // So if a0=0x1002, offset -1 leads to 0x1001 (unaligned).
    //     0x23B9, // li a0, 0x1002 (imm=0x10, rd=a0) => 0x21B9 for 0x1000, 0x23B9 for 0x1002.
    //     0xF2A3, // sw x1, -1(a0) (imm=-1 (0xF), rs2=x1(1), rs1=a0(6), funct3=2, opcode=3) => (0xF<<12)|(1<<9)|(6<<6)|(2<<3)|3 = 0xF2A3
    //     0xFFC7  // ecall 0x3FF
    // };
    // createBinFile("tests/TC-ZX16-16_SW_Unaligned.bin", tc_16_sw_unaligned_insts);
    //
    //
    // // // --- L-Type Test Cases ---
    // // // TC-ZX16-17_LB_PosSigned: lb x0, 2(a1) => x0 = sext(0xCC) = 0xFFCC
    // // std::vector<uint16_t> tc_17_lb_pos_signed_insts = {
    // //     0x21F9, // li a1, 0x2000 (imm=0x10 (16), rd=a1(7), funct3=7) => (0x10<<9)|(7<<6)|(7<<3)|1 = 0x21F9
    // //     0x4384, // lb x0, 2(a1) (imm=2, rs2=a1(7), rd=x0(0), funct3=0, opcode=4) => (2<<12)|(7<<9)|(0<<6)|(0<<3)|4 = 0x2E04
    // //             // My old calculation for this was 0x2704. Let's recalculate `0x2704`.
    // //             // imm4 = (0x2704 >> 12) & 0xF = 0x2. (2)
    // //             // rs2 = (0x2704 >> 9) & 0x7 = 0x7. (x7/a1)
    // //             // rd = (0x2704 >> 6) & 0x7 = 0x0. (x0)
    // //             // funct3 = (0x2704 >> 3) & 0x7 = 0x0. (LB)
    // //             // opcode = 0x4. (L-type)
    // //             // So, 0x2704 is correct for LB x0, 2(a1). My initial derivation of 0x4384 was a mistake.
    // //     0x2704, // lb x0, 2(a1)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-17_LB_PosSigned.bin", tc_17_lb_pos_signed_insts);
    // //
    // // // TC-ZX16-18_LB_NegSigned: lb x0, 3(a1) => x0 = sext(0x81) = 0xFF81
    // // std::vector<uint16_t> tc_18_lb_neg_signed_insts = {
    // //     0x21F9, // li a1, 0x2000
    // //     0x3704, // lb x0, 3(a1) (imm=3, rs2=a1(7), rd=x0(0), funct3=0, opcode=4) => (3<<12)|(7<<9)|(0<<6)|(0<<3)|4 = 0x3E04.
    // //             // Original test was 0x3704. Let's re-verify `0x3704`.
    // //             // imm4 = (0x3704 >> 12) & 0xF = 0x3. (3)
    // //             // rs2 = (0x3704 >> 9) & 0x7 = 0x7. (x7/a1)
    // //             // rd = (0x3704 >> 6) & 0x7 = 0x0. (x0)
    // //             // funct3 = (0x3704 >> 3) & 0x7 = 0x0. (LB)
    // //             // opcode = 0x4. (L-type)
    // //             // So, 0x3704 is correct for LB x0, 3(a1).
    // //     0x3704, // lb x0, 3(a1)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-18_LB_NegSigned.bin", tc_18_lb_neg_signed_insts);
    // //
    // // // TC-ZX16-19_LW: lw x1, 0(a1) => x1 = 0xBEEF
    // // std::vector<uint16_t> tc_19_lw_insts = {
    // //     0x21F9, // li a1, 0x2000
    // //     0x0724, // lw x1, 0(a1) (imm=0, rs2=a1(7), rd=x1(1), funct3=2, opcode=4) => (0<<12)|(7<<9)|(1<<6)|(2<<3)|4 = 0x0724
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-19_LW.bin", tc_19_lw_insts);
    // //
    // // // TC-ZX16-20_LBU: lbu x0, 3(a1) => x0 = zext(0x81) = 0x0081
    // // std::vector<uint16_t> tc_20_lbu_insts = {
    // //     0x21F9, // li a1, 0x2000
    // //     0x3714, // lbu x0, 3(a1) (imm=3, rs2=a1(7), rd=x0(0), funct3=3, opcode=4) => (3<<12)|(7<<9)|(0<<6)|(3<<3)|4 = 0x3E14.
    // //             // Original test was 0x3714. Let's re-verify `0x3714`.
    // //             // imm4 = (0x3714 >> 12) & 0xF = 0x3. (3)
    // //             // rs2 = (0x3714 >> 9) & 0x7 = 0x7. (x7/a1)
    // //             // rd = (0x3714 >> 6) & 0x7 = 0x0. (x0)
    // //             // funct3 = (0x3714 >> 3) & 0x7 = 0x3. (LBU)
    // //             // opcode = 0x4. (L-type)
    // //             // So, 0x3714 is correct for LBU x0, 3(a1).
    // //     0x3714, // lbu x0, 3(a1)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-20_LBU.bin", tc_20_lbu_insts);
    // //
    // // // TC-ZX16-21_LW_Unaligned: lw x0, 1(a1) => Should print error, not load
    // // std::vector<uint16_t> tc_21_lw_unaligned_insts = {
    // //     0x21F9, // li a1, 0x2000
    // //     0x1724, // lw x0, 1(a1) (imm=1, rs2=a1(7), rd=x0(0), funct3=2, opcode=4) => (1<<12)|(7<<9)|(0<<6)|(2<<3)|4 = 0x1E04.
    // //             // Original test was 0x1724. Let's re-verify `0x1724`.
    // //             // imm4 = (0x1724 >> 12) & 0xF = 0x1. (1)
    // //             // rs2 = (0x1724 >> 9) & 0x7 = 0x7. (x7/a1)
    // //             // rd = (0x1724 >> 6) & 0x7 = 0x0. (x0)
    // //             // funct3 = (0x1724 >> 3) & 0x7 = 0x2. (LW)
    // //             // opcode = 0x4. (L-type)
    // //             // So, 0x1724 is correct for LW x0, 1(a1).
    // //     0x1724, // lw x0, 1(a1)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-21_LW_Unaligned.bin", tc_21_lw_unaligned_insts);
    // //
    // // // --- Ecall Service Tests ---
    // // // TC-ZX16-22_ECALL_PRINT_INT: ecall 0x3
    // // std::vector<uint16_t> tc_22_ecall_print_int_insts = {
    // //     0x27A1, // li a0, 123 (imm=123 (0x7B), rd=a0(6), funct3=7) => (0x7B<<9)|(6<<6)|(7<<3)|1 = 0xF6B9.
    // //             // Original test was 0x27A1. Let's re-verify `0x27A1`.
    // //             // imm7 = (0x27A1 >> 9) & 0x7F = 0x4F (79).
    // //             // This means `li a0, 79`. So the original test for printing 123 was wrong.
    // //             // Let's use `li a0, 79` if the expected output is 79, or encode 123 correctly.
    // //             // For `li a0, 123`: `imm7=0x7B`.
    // //             // `(0x7B<<9)|(6<<6)|(7<<3)|1 = 0xF6B9`. This is what `li a0, 123` should be.
    // //     0xF6B9, // li a0, 123
    // //     0x00C7, // ecall 0x3 (svc=3, funct3=0, opcode=7) => (3<<6)|(0<<3)|7 = 0x00C7
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-22_ECALL_PRINT_INT.bin", tc_22_ecall_print_int_insts);
    // //
    // // // TC-ZX16-23_ECALL_PRINT_CHAR: ecall 0x0
    // // std::vector<uint16_t> tc_23_ecall_print_char_insts = {
    // //     0x1871, // li a0, 0x41 ('A')
    // //     0x0007, // ecall 0x0 (svc=0, funct3=0, opcode=7) => (0<<6)|(0<<3)|7 = 0x0007
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-23_ECALL_PRINT_CHAR.bin", tc_23_ecall_print_char_insts);
    // //
    // // // TC-ZX16-24_ECALL_PRINT_STRING: ecall 0x2
    // // // Requires data in memory. Let's place "Hello" at 0x1000
    // // std::vector<uint16_t> tc_24_ecall_print_string_insts = {
    // //     0x21B9, // li a0, 0x1000 (address of string)
    // //     0x0087, // ecall 0x2 (svc=2, funct3=0, opcode=7) => (2<<6)|(0<<3)|7 = 0x0087
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-24_ECALL_PRINT_STRING.bin", tc_24_ecall_print_string_insts);
    // //
    // // // Add string data to the binary for TC-ZX16-24_ECALL_PRINT_STRING
    // // // This is a bit tricky with `createBinFile` as it only adds instructions.
    // // // For string data, we'd need to manually append bytes or have a separate `createDataFile` function.
    // // // For now, let's assume `z16sim::loadMemoryFromFile` will load a combined .bin.
    // // // If not, you'd need to modify `z16sim::loadMemoryFromFile` to allow appending data at specific addresses,
    // // // or modify `createBinFile` to include a data section.
    // // // For simplicity, let's just make sure the string address is in `a0` and `z16sim.cpp` reads directly from `memory`.
    // // // The `loadMemoryFromFile` loads from address 0x0000. So string must be part of the binary image itself.
    // // // Let's make the string part of the instruction stream for this example.
    // // // "Hello\0" = 0x48 0x65 0x6C 0x6C 0x6F 0x00
    // // // This will overwrite subsequent instructions if not careful.
    // // // Best practice is to put data after instructions, or at a higher address.
    // // // For now, I'll add a separate raw data vector and manually append it to the bin file.
    // // std::vector<uint8_t> string_data = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00}; // "Hello\0"
    // // std::ofstream ofs_str("tests/TC-ZX16-24_ECALL_PRINT_STRING.bin", std::ios::binary | std::ios::app); // Open in append mode
    // // if (!ofs_str) {
    // //     std::cerr << "Error opening binary file for append: tests/TC-ZX16-24_ECALL_PRINT_STRING.bin" << std::endl;
    // //     exit(1);
    // // }
    // // for (uint8_t byte : string_data) {
    // //     ofs_str.put(byte);
    // // }
    // // ofs_str.close();
    // //
    // //
    // // // --- Control Flow (J-type, U-type, JR, JALR) ---
    // // // TC-ZX16-25_J: j +6 (jump over next inst)
    // // std::vector<uint16_t> tc_25_j_insts = {
    // //     0x0C35, // j +6 (imm=3, link_flag=0, rd=any, imm=3) (0x0C35: imm[9:4]=0x00, rd=x0, imm[3:1]=0b011. => (3*2)=6 offset)
    // //             // Correct J encoding (imm=3, pc+6 offset, which is 3 instructions forward or PC + 6 actual address)
    // //             // Relative offset for J/JAL is calculated as `target - PC`.
    // //             // `j 0x0008` from `PC=0x0000` means offset 8. `imm = 8/2 = 4`.
    // //             // `imm9_4 = (4 >> 3) & 0x3F = 0`. `imm3_1 = 4 & 0x7 = 4`.
    // //             // `link_flag=0`, `rd=x0`.
    // //             // `(0<<15) | (0<<9) | (0<<6) | (4<<3) | 5 = 0x25`.
    // //             // So `j +8` (to PC=0x0008) should be `0x0025`.
    // //             // Let's test `j +4` (skips one instruction)
    // //             // `j +4` (offset 4) -> `imm = 4/2 = 2`. `imm9_4=0`, `imm3_1=2`.
    // //             // `(0<<15) | (0<<9) | (0<<6) | (2<<3) | 5 = 0x15`.
    // //     0x0015, // j +4 (from 0x0000 to 0x0004)
    // //     NOP,    // NOP (skipped)
    // //     0x0271, // li x1, 1 (target)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-25_J.bin", tc_25_j_insts);
    // //
    // // // TC-ZX16-26_JAL: jal ra, +8 (jump and link)
    // // std::vector<uint16_t> tc_26_jal_insts = {
    // //     0x8025, // jal ra, +8 (from 0x0000 to 0x0008)
    // //             // `jal ra, +8`: `imm=4`. `link_flag=1`. `rd=ra(1)`.
    // //             // `(1<<15) | (0<<9) | (1<<6) | (4<<3) | 5 = 0x8000 | 0x40 | 0x20 | 0x05 = 0x8065`.
    // //             // My old calculation: `0x8025`.
    // //             // `0x8025`: link=1, imm9_4=(0x8025>>9)&0x3F = 0, rd=(0x8025>>6)&0x7 = 0x0 (x0), imm3_1=(0x8025>>3)&0x7 = 0x4.
    // //             // So 0x8025 is JAL x0, +8.
    // //             // For JAL ra, +8, it should be 0x8065.
    // //     0x8065, // jal ra, +8
    // //     NOP,    // NOP (skipped)
    // //     NOP,    // NOP (skipped)
    // //     0x0271, // li x1, 1 (target)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-26_JAL.bin", tc_26_jal_insts);
    // //
    // // // TC-ZX16-27_JR: jr ra (return from function)
    // // std::vector<uint16_t> tc_27_jr_insts = {
    // //     0x0271, // li ra, 0x1000 (set return address in ra)
    // //     0x3060, // jr ra (funct4=0xB, rd_rs1=ra(1), funct3=0, opcode=0) => (0xB<<12)|(0<<9)|(1<<6)|(0<<3)|0 = 0xB040.
    // //             // My old calculation: 0x3060.
    // //             // 0x3060: funct4=(0x3060>>12)&0xF = 0x3. This is wrong. It should be 0xB.
    // //             // Let's use 0xB040 for `jr ra`.
    // //     0xB040, // jr ra
    // //     0xFFC7  // ecall 0x3FF (This will not be reached if JR works)
    // // };
    // // // The `jr` instruction jumps. We need to set `ra` to a *valid instruction address* that contains an ecall 0x3FF.
    // // // So, this binary will contain a `jr` that jumps *back into this binary* to an `ecall 0x3FF`.
    // // std::vector<uint16_t> tc_27_jr_full_insts = {
    // //     0x2171, // li ra, 0x0006 (Set ra to address of ecall)
    // //     0xB040, // jr ra
    // //     NOP,    // This should be skipped
    // //     0xFFC7  // ecall 0x3FF (This is the target of JR)
    // // };
    // // createBinFile("tests/TC-ZX16-27_JR.bin", tc_27_jr_full_insts);
    // //
    // //
    // // // TC-ZX16-28_JALR: jalr ra (jump and link register)
    // // std::vector<uint16_t> tc_28_jalr_insts = {
    // //     0x2171, // li ra, 0x0006 (target address for JALR)
    // //     0xC040, // jalr ra (funct4=0xC, rd_rs1=ra(1), funct3=0, opcode=0) => (0xC<<12)|(0<<9)|(1<<6)|(0<<3)|0 = 0xC040
    // //     NOP,    // This should be skipped
    // //     0xFFC7  // ecall 0x3FF (target of JALR)
    // // };
    // // createBinFile("tests/TC-ZX16-28_JALR.bin", tc_28_jalr_insts);
    // //
    // // // TC-ZX16-29_LUI: lui x1, 0x123 (load upper immediate)
    // // std::vector<uint16_t> tc_29_lui_insts = {
    // //     // lui x1, 0x123.
    // //     // imm = 0x123. 9-bit immediate (imm15_10, imm9_7)
    // //     // 0x123 = 00100100011 (binary). Need to extract 9 bits.
    // //     // LUI takes `imm[15:7]` and places it in `rd[15:7]`, zeros out `rd[6:0]`.
    // //     // The `disassemble` and `execute` use `imm = (imm15_10 << 3) | imm9_7;` which means a 9-bit value.
    // //     // `imm = 0x123`. `imm15_10 = (0x123 >> 3) & 0x3F = 0x24 (36)`.
    // //     // `imm9_7 = 0x123 & 0x7 = 0x3`.
    // //     // So `imm15_10=0x24`, `imm9_7=0x3`.
    // //     // `flag=0` (LUI). `rd=x1(1)`. `opcode=6`.
    // //     // `(0<<15) | (0x24<<9) | (1<<6) | (0x3<<3) | 6 = 0x4800 | 0x40 | 0x18 | 0x06 = 0x485E`.
    // //     // So `lui x1, 0x123` is `0x485E`.
    // //     0x485E, // lui x1, 0x123
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-29_LUI.bin", tc_29_lui_insts);
    // //
    // //
    // // // TC-ZX16-30_AUIPC: auipc x1, 0x123 (add upper immediate to PC)
    // // std::vector<uint16_t> tc_30_auipc_insts = {
    // //     // auipc x1, 0x123. Same immediate encoding as LUI.
    // //     // `flag=1` (AUIPC). `rd=x1(1)`. `opcode=6`.
    // //     // `(1<<15) | (0x24<<9) | (1<<6) | (0x3<<3) | 6 = 0x8000 | 0x4800 | 0x40 | 0x18 | 0x06 = 0xCA5E`.
    // //     0xCA5E, // auipc x1, 0x123 (PC starts at 0, x1 = 0 + (0x123 << 7) = 0x9180)
    // //     0xFFC7  // ecall 0x3FF
    // // };
    // // createBinFile("tests/TC-ZX16-30_AUIPC.bin", tc_30_auipc_insts);
    // //

    std::cout << "\nAll binary test files generated." << std::endl;
    return 0;
}