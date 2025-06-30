#include "z16sim.h"

#define MEM_SIZE 65536  // 64KB memory
uint16_t regs[8] = {0};
uint16_t pc = 0;
unsigned char memory[MEM_SIZE] = {0};
bool debug = false;
const char* regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

z16sim::z16sim() {
    std::memset(memory, 0, MEM_SIZE);
    std::memset(regs, 0, sizeof(regs));
}


// -----------------------
// Disassembly Function
// -----------------------
//
// Decodes a 16-bit instruction 'inst' (fetched at address 'pc') and writes a human‑readable
// string to 'buf' (of size bufSize). This decoder uses the opcode (bits [2:0]) to distinguish
// among R‑, I‑, B‑, L‑, J‑, U‑, and System instructions.
void z16sim::disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize) {
     uint8_t opcode = inst & 0x7;
    switch(opcode) {
         case 0x0: { // R-type
            uint8_t funct4  = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            if (funct4 == 0x0 && funct3 == 0x0)
                snprintf(buf, bufSize, "add %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x1 && funct3 == 0x0)
                snprintf(buf, bufSize, "sub %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x2 && funct3 == 0x1)
                snprintf(buf, bufSize, "slt %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x3 && funct3 == 0x2)
                snprintf(buf, bufSize, "sltu %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x4 && funct3 == 0x3)
                snprintf(buf, bufSize, "sll %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x5 && funct3 == 0x3)
                snprintf(buf, bufSize, "srl %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x6 && funct3 == 0x3)
                snprintf(buf, bufSize, "sra %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x7 && funct3 == 0x4)
                snprintf(buf, bufSize, "or %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x8 && funct3 == 0x5)
                snprintf(buf, bufSize, "and %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x9 && funct3 == 0x6)
                snprintf(buf, bufSize, "xor %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0xA && funct3 == 0x7)
                snprintf(buf, bufSize, "mv %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0xB && funct3 == 0x0)
                snprintf(buf, bufSize, "jr %s", regNames[rd_rs1]);
            else if (funct4 == 0xC && funct3 == 0x0)
                snprintf(buf, bufSize, "jalr %s", regNames[rd_rs1]);
            else
                snprintf(buf, bufSize, "unknown R-type");
            break;
        }

        case 0x1: { // I-type
            int16_t imm = (inst >> 9) & 0x7F;
            // Sign-extend if imm is negative
            if (imm & 0x40) imm |= 0xFF80;

            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "addi %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "slti %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "sltui %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x3) {
                // Check bits 6:4 of imm for shift type
                uint8_t shift_type = (imm >> 4) & 0x7;
                uint8_t shamt = imm & 0xF;
                if (shift_type == 0x1)
                    snprintf(buf, bufSize, "slli %s, %d", regNames[rd_rs1], shamt);
                else if (shift_type == 0x2)
                    snprintf(buf, bufSize, "srli %s, %d", regNames[rd_rs1], shamt);
                else if (shift_type == 0x4)
                    snprintf(buf, bufSize, "srai %s, %d", regNames[rd_rs1], shamt);
                else
                    snprintf(buf, bufSize, "unknown shift imm");
            }
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "ori %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x5)
                snprintf(buf, bufSize, "andi %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x6)
                snprintf(buf, bufSize, "xori %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "li %s, %d", regNames[rd_rs1], imm);
            else
                snprintf(buf, bufSize, "unknown I-type");
            break;
        }

        case 0x2: { // B-type (branch): [15:12] offset[4:1] | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
                uint8_t imm_high = (inst >> 12) & 0xF; // imm[4:1]
                uint8_t rs2 = (inst >> 9) & 0x7;
                uint8_t rs1 = (inst >> 6) & 0x7;
                uint8_t funct3 = (inst >> 3) & 0x7;

                // Reconstruct 5-bit signed offset (imm[0] = 0, so multiply by 2)
                int16_t offset = (imm_high << 1);
                // Sign extend if bit 4 is set
                if (offset & 0x10) offset |= 0xFFE0;

                uint16_t target_addr = pc + offset;

                if (funct3 == 0x0)
                    snprintf(buf, bufSize, "beq %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x1)
                    snprintf(buf, bufSize, "bne %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x2)
                    snprintf(buf, bufSize, "blt %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x3)
                    snprintf(buf, bufSize, "bge %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x4)
                    snprintf(buf, bufSize, "bltu %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x5)
                    snprintf(buf, bufSize, "bgeu %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
                else if (funct3 == 0x6)
                    snprintf(buf, bufSize, "bz %s, 0x%04X", regNames[rs1], target_addr); // rs2 ignored
                else if (funct3 == 0x7)
                    snprintf(buf, bufSize, "bnz %s, 0x%04X", regNames[rs1], target_addr); // rs2 ignored
                else
                    snprintf(buf, bufSize, "unknown B-type");

                break;
        }

        case 0x3: { // S-type (branch): [15:12] offset[4:1] | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
                uint8_t imm4 = (inst >> 12) & 0xF; // imm[3:0]
                uint8_t rs2 = (inst >> 9) & 0x7;   // data register
                uint8_t rs1 = (inst >> 6) & 0x7;   // base register
                uint8_t funct3 = (inst >> 3) & 0x7;

                // Sign extend 4-bit immediate
                int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;

                if (funct3 == 0x0)
                    snprintf(buf, bufSize, "sb %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
                else if (funct3 == 0x2)
                    snprintf(buf, bufSize, "sw %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
                else
                    snprintf(buf, bufSize, "unknown S-type");

                break;
        }

        case 0x4: { // L-type (branch): [15:12] offset[4:1] | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
                uint8_t imm4 = (inst >> 12) & 0xF; // imm[3:0]
                uint8_t rs2 = (inst >> 9) & 0x7;   // base register
                uint8_t rd = (inst >> 6) & 0x7;    // destination register
                uint8_t funct3 = (inst >> 3) & 0x7;

                // Sign extend 4-bit immediate
                int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;

                if (funct3 == 0x0)
                    snprintf(buf, bufSize, "lb %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
                else if (funct3 == 0x2)
                    snprintf(buf, bufSize, "lw %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
                else if (funct3 == 0x3)
                    snprintf(buf, bufSize, "lbu %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
                else
                    snprintf(buf, bufSize, "unknown L-type");

                break;
        }


        //J-Type (opcode = 101)
        case 0x5: { //J type : [15] f | [14:9] imm[5:0] | [8:6] rd | [5:3] imm[8:6] | [2:0] opcode
             uint8_t f = (inst >> 15) & 0x1;              // Extract 'f' (bit 15)
             uint8_t imm9to4 = (inst >> 9) & 0x3F;        // Extract I[9:4] (bits 14:9)
             uint8_t rd = (inst >> 6) & 0x7;              // Extract 'rd' (bits 8:6)
             uint8_t imm3to1 = (inst >> 3) & 0x7;

             int16_t imm = (imm9to4 << 4) | (imm3to1 << 1);

             // Sign extension
             if (imm & 0x200) { // Check if bit 9 is set
                 imm |= 0xFC00; // Sign extend to a full 16-bit value
             }
             // Scale by 2 for 16-bit instruction alignment

             if (f == 0) {
                 // Disassemble 'j' instruction
                 printf("j %d\n", imm);
             } else if (f == 1) {

                 // Disassemble 'jal' instruction

                 printf("jal %s, %d\n", regNames[rd], imm);
             }
             else
                 printf("Unknown Instruction\n");

             break;
        }


        // U-Type (opcode = 110)
        case 0x6: { // U-Type
             uint8_t f = (inst >> 15) & 0x1;
             uint8_t rd  = (inst >> 6) & 0x7;
             uint16_t I  = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8);
             if(f==0)
                 printf("lui %s, 0x%X\n", regNames[rd], I);    // prints the number in hexadecimal
             else
                 printf("auipc %s, 0x%X\n", regNames[rd], I);  // prints the number in hexadecimal
             break;
        }
        // SYS-Type (opcode = 111)
        case 0x7: { // SYS-Type: [15:6] svc | [5:3] 000 | [2:0] opcode
            uint16_t svc = (inst >> 6) & 0x3FF; // (10-bit system-call number)
            uint8_t func3 = (inst >> 3) & 0x7; // 000

             if (func3 == 0x0)
                 snprintf(buf, bufSize, "ecall 0x%03X", svc);
             else
                 snprintf(buf, bufSize, "unknown SYS-type");
             break;
        }

        default:
            snprintf(buf, bufSize, "Unknown opcode");
            break;
    }
}

// -----------------------
// Instruction Execution
// -----------------------
//
// Executes the instruction 'inst' (a 16-bit word) by updating registers, memory, and PC.
// Returns 1 to continue simulation or 0 to terminate (if ecall 3 is executed).
int z16sim::executeInstruction(uint16_t inst) {
    uint8_t opcode = inst & 0x7;
    int pcUpdated = 0; // flag: if instruction updated PC directly

    switch(opcode) {
        case 0x0: { // R-type
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            if (funct4 == 0x0 && funct3 == 0x0) // ADD
                regs[rd_rs1] = regs[rd_rs1] + regs[rs2];
            else if (funct4 == 0x1 && funct3 == 0x0) // SUB
                regs[rd_rs1] = regs[rd_rs1] - regs[rs2];
            else if (funct4 == 0x2 && funct3 == 0x1) // SLT
                regs[rd_rs1] = ((int16_t)regs[rd_rs1] < (int16_t)regs[rs2]) ? 1 : 0;
            else if (funct4 == 0x3 && funct3 == 0x2) // SLTU
                regs[rd_rs1] = (regs[rd_rs1] < regs[rs2]) ? 1 : 0;
            else if (funct4 == 0x4 && funct3 == 0x3) // SLL
                regs[rd_rs1] = regs[rd_rs1] << (regs[rs2] & 0xF); // Limit shift to 0–15
            else if (funct4 == 0x5 && funct3 == 0x3) // SRL
                regs[rd_rs1] = regs[rd_rs1] >> (regs[rs2] & 0xF);
            else if (funct4 == 0x6 && funct3 == 0x3) // SRA
                regs[rd_rs1] = ((int16_t)regs[rd_rs1]) >> (regs[rs2] & 0xF);
            else if (funct4 == 0xA && funct3 == 0x7) // MV
                regs[rd_rs1] = regs[rs2];
            else if (funct4 == 0x8 && funct3 == 0x5) // AND
                regs[rd_rs1] = regs[rd_rs1] & regs[rs2];
            else if (funct4 == 0x7 && funct3 == 0x4) // OR
                regs[rd_rs1] = regs[rd_rs1] | regs[rs2];
            else if (funct4 == 0x9 && funct3 == 0x6) // XOR
                regs[rd_rs1] = regs[rd_rs1] ^ regs[rs2];
            else if (funct4 == 0xB && funct3 == 0x0) { // JR
                pc = regs[rd_rs1];
                pcUpdated = 1;
            }
            else if (funct4 == 0xC && funct3 == 0x0) { // JALR
                uint16_t new_pc = regs[rd_rs1];
                regs[rd_rs1] = pc + 2;
                pc = new_pc;
                pcUpdated = 1;
            }

            break; // CRITICAL FIX: Added missing break statement
        }

        case 0x1: { // I-type: [15:9] imm[6:0] | [8:6] rd | [5:3] funct3 | [2:0] opcode
            uint8_t imm7     = (inst >> 9) & 0x7F;
            uint8_t rd       = (inst >> 6) & 0x7;
            uint8_t funct3   = (inst >> 3) & 0x7;

            // Sign-extended immediate
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7;

            switch (funct3) {
                case 0x0: // ADDI
                    regs[rd] += simm;
                break;
                case 0x1: // SLTI
                    regs[rd] = ((int16_t)regs[rd] < simm) ? 1 : 0;
                break;
                case 0x2: // SLTUI
                    regs[rd] = (regs[rd] < (uint16_t)simm) ? 1 : 0;
                break;
                case 0x3: { // Shift instructions
                    uint8_t shamt       = imm7 & 0xF;         // shift amount
                    uint8_t shift_type  = (imm7 >> 4) & 0x7;  // shift function selector

                    if (shift_type == 0x1) {      // SLLI
                        regs[rd] <<= shamt;
                    } else if (shift_type == 0x2) { // SRLI
                        regs[rd] = ((uint16_t)regs[rd]) >> shamt;
                    } else if (shift_type == 0x4) { // SRAI
                        regs[rd] = (int16_t)regs[rd] >> shamt;
                    }
                    break;
                }
                case 0x4: // ORI
                    regs[rd] |= simm;
                break;
                case 0x5: // ANDI
                    regs[rd] &= simm;
                break;
                case 0x6: // XORI
                    regs[rd] ^= simm;
                break;
                case 0x7:
    // printf("LI executed: x%d = %d\n", rd, simm);
    regs[rd] = simm;
    break;

                default:
                    printf("Unknown I-type funct3: 0x%X\n", funct3);
                break;
            }

            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t imm_high = (inst >> 12) & 0xF; // imm[4:1]
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Reconstruct 5-bit signed offset (imm[0] = 0, so multiply by 2)
            int16_t offset = (imm_high << 1);
            // Sign extend if bit 4 is set
            if (offset & 0x10) offset |= 0xFFE0;

            bool branch_taken = false;

            if (funct3 == 0x0) { // BEQ
                branch_taken = (regs[rs1] == regs[rs2]);
            }
            else if (funct3 == 0x1) { // BNE
                branch_taken = (regs[rs1] != regs[rs2]);
            }
            else if (funct3 == 0x2) { // BLT (signed)
                branch_taken = ((int16_t)regs[rs1] < (int16_t)regs[rs2]);
            }
            else if (funct3 == 0x3) { // BGE (signed)
                branch_taken = ((int16_t)regs[rs1] >= (int16_t)regs[rs2]);
            }
            else if (funct3 == 0x4) { // BLTU (unsigned)
                branch_taken = (regs[rs1] < regs[rs2]);
            }
            else if (funct3 == 0x5) { // BGEU (unsigned)
                branch_taken = (regs[rs1] >= regs[rs2]);
            }
            else if (funct3 == 0x6) { // BZ (branch if zero)
                branch_taken = (regs[rs1] == 0);
            }
            else if (funct3 == 0x7) { // BNZ (branch if not zero)
                branch_taken = (regs[rs1] != 0);
            }

            if (branch_taken) {
                pc += offset;
                pcUpdated = 1;
            }


            break;
        }

        case 0x3: { // S-type (store)
            uint8_t imm4 = (inst >> 12) & 0xF; // imm[3:0]
            uint8_t rs2 = (inst >> 9) & 0x7;   // data register
            uint8_t rs1 = (inst >> 6) & 0x7;   // base register
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Sign extend 4-bit immediate
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint16_t addr = regs[rs1] + offset;

            // Check bounds
            if (addr >= MEM_SIZE) {
                printf("Store address 0x%04X out of bounds\n", addr);
                break;
            }

            if (funct3 == 0x0) { // SB (store byte)
                memory[addr] = regs[rs2] & 0xFF;
            }
            else if (funct3 == 0x2) { // SW (store word - 16 bits)
                // Check word alignment
                if (addr & 0x1) {
                    printf("Store word address 0x%04X not word-aligned\n", addr);
                    break;
                }
                if (addr + 1 >= MEM_SIZE) {
                    printf("Store word address 0x%04X out of bounds\n", addr);
                    break;
                }
                // Little-endian storage
                memory[addr] = regs[rs2] & 0xFF;
                memory[addr + 1] = (regs[rs2] >> 8) & 0xFF;
            }

            break;
        }
        case 0x4: { // L-type (load)
            uint8_t imm4 = (inst >> 12) & 0xF; // imm[3:0]
            uint8_t rs2 = (inst >> 9) & 0x7;   // base register
            uint8_t rd = (inst >> 6) & 0x7;    // destination register
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Sign extend 4-bit immediate
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint16_t addr = regs[rs2] + offset;

            // Check bounds
            if (addr >= MEM_SIZE) {
                printf("Load address 0x%04X out of bounds\n", addr);
                break;
            }

            if (funct3 == 0x0) { // LB (load byte, sign-extended)
                uint8_t byte_val = memory[addr];
                regs[rd] = (byte_val & 0x80) ? (byte_val | 0xFF00) : byte_val;
            }
            else if (funct3 == 0x2) { // LW (load word - 16 bits)
                // Check word alignment
                if (addr & 0x1) {
                    printf("Load word address 0x%04X not word-aligned\n", addr);
                    break;
                }
                if (addr + 1 >= MEM_SIZE) {
                    printf("Load word address 0x%04X out of bounds\n", addr);
                    break;
                }
                // Little-endian load
                regs[rd] = memory[addr] | (memory[addr + 1] << 8);
            }
            else if (funct3 == 0x3) { // LBU (load byte unsigned)
                regs[rd] = memory[addr];
            }

            break;
        }

        case 0x5: { // J-type (jump)
            uint8_t link_flag = (inst >> 15) & 0x1; // (0 = J, 1 = JAL)
            int16_t imm9_4 = (inst >> 9) & 0x3F; // (high 6 bits of 10-bit signed offset, imm[0] = 0)
            uint8_t rd = (inst >> 6) & 0x7; // (link register for JAL)
            int16_t imm3_1 = (inst >> 3) & 0x7; // (low 3 bits of offset)

            int16_t imm = (imm9_4 << 3) | imm3_1; // combine immediates

            // sign extend immedate if needed
            if (imm & (1 << 8))
                imm = imm | 0xFE00;

            // calculate PC relative target address (PC + 2)
            uint16_t target = pc + (imm * 2);

            if (link_flag) // JAL
                // x[rd] ← PC + 2; PC ← PC + offset
                regs[rd] = pc + 2;

            pc = target;
            pcUpdated = 1;
            break;

        }

        case 0x6: { // U-type
            uint8_t flag = (inst >> 15) & 0x1; // (0 = LUI, 1 = AUIPC)
            int16_t imm15_10 = (inst >> 9) & 0x3F; // (high 6 bits of immediate)
            uint8_t rd = (inst >> 6) & 0x7;
            int16_t imm9_7 = (inst >> 3) & 0x7; // (mid 3 bits of immediate)

            int16_t imm = (imm15_10 << 3) | imm9_7; // combine immediates

            // sign extend immedate if needed
            if (imm & (1 << 8))
                imm = imm | 0xFE00;

            // shift immediate to upper bits
            imm = imm << 7;

            if (flag) // AUIPC rd ← PC + (imm[15:7] << 7)
                regs[rd] = pc + imm;
            else // LUI rd ← (imm[15:7] << 7)
            regs[rd] = imm;
            break;

        }
        case 0x7: { // System instruction (ecall)
            uint16_t svc = (inst >> 6) & 0x3FF; // (10-bit system-call number)
            uint8_t func3 = (inst >> 3) & 0x7; // 000
            if (func3 == 0x0) {
                if (svc == 0x0) // Print character syscall
                    printf("%c", regs[6] & 0xFF); // a0 is in regs[6]
                else if (svc == 0x1) // Read char into a0
                    regs[6] = getchar() & 0xFF;
                else if (svc == 0x2) // Print string syscall
                    printf("%s", (char*)&memory[regs[6]]);
                else if (svc == 0x3) // # Print decimal
                    printf("%d", (int16_t)regs[6]);
                else if (svc == 0x3FF) // Exit program syscall
                    return 0;
                break;
                // else
                //     printf("Unknown ecall: 0x%03X\n", svc);
                // break;
            }
            printf("Invalid system instruction: 0x%X\n", func3);
            break;
        }

        default:
            printf("Unknown instruction opcode 0x%X\n", opcode);
            break;
    }
    if(!pcUpdated)
        pc += 2; // default: move to next instruction
    return 1;
}

// -----------------------
// Memory Loading
// -----------------------
//
//Loads the binary machine code image from the specified file into simulated memory.
void z16sim::loadMemoryFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        perror("Error opening binary file");
        exit(1);
    }
    size_t n = fread(memory, 1, MEM_SIZE, fp);
    fclose(fp);
    printf("Loaded %zu bytes into memory\n", n);
}

bool z16sim::cycle() {
    char disasmBuf[128];

    // Check if PC is within valid memory bounds for instruction fetch
    // An instruction is 2 bytes, so pc + 1 must be within MEM_SIZE
    if (pc + 1 >= MEM_SIZE) {
        printf("Instruction fetch at PC 0x%04X would go out of bounds.\n", pc);
        return false;
    }

    // Check if PC is word-aligned (even address for 16-bit instructions)
    if (pc & 0x1) {
        printf("PC 0x%04X not aligned to instruction boundary.\n", pc);
        return false;
    }

    // Check if trying to execute from MMIO region (0xF000-0xFFFF)
    if (pc >= 0xF000) {
        printf("Cannot execute from MMIO region at PC 0x%04X.\n", pc);
        return false;
    }

    // Fetch a 16-bit instruction from memory (little-endian)
    uint16_t inst = memory[pc] | (memory[pc + 1] << 8);

    // Disassemble and print the instruction
    disassemble(inst, pc, disasmBuf, sizeof(disasmBuf));
    printf("0x%04X: %04X    %s\n", pc, inst, disasmBuf);

    // Execute the instruction
    if (!executeInstruction(inst)) {
        return false; // Instruction requested termination (e.g., ecall 0x3FF)
    }

    return true; // Continue execution
}
void z16sim::reset() {
    for (int i = 0; i < 8; ++i)
        regs[i] = 0;
    pc = 0;
}


bool z16sim::updatePC(uint16_t new_pc, const char* instruction_name)
{
    // Check if new PC is within valid memory bounds for instruction fetch
    // An instruction is 2 bytes, so new_pc + 1 must be within MEM_SIZE.
    if (new_pc >= MEM_SIZE - 1) {
        printf("%s: PC 0x%04X out of bounds for next instruction fetch\n", instruction_name, new_pc);
        return false;
    }

    // Check if PC is word-aligned (even address for 16-bit instructions)
    if (new_pc & 0x1) {
        printf("%s: PC 0x%04X not aligned to instruction boundary\n", instruction_name, new_pc);
        return false;
    }

    // Check if trying to execute from MMIO region (0xF000-0xFFFF)
    if (new_pc >= 0xF000) {
        printf("%s: Cannot execute from MMIO region 0x%04X\n", instruction_name, new_pc);
        return false;
    }

    pc = new_pc; // Update the class member pc
    return true;
}

