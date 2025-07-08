#include "z16sim.h" 
#include "graphics.h"
#include <iostream>  // For std::cout, std::cerr
#include <iomanip>   // For std::hex, std::setw, std::setfill
#include <stdexcept> // For std::runtime_error
#include <cstdio>    // For snprintf, fopen, fread, ferror, fclose
#include <cstring>

// Initialize static member (outside class definition)
const char* z16sim::regNames[z16sim::NUM_REGS] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};


// Constructor
z16sim::z16sim() : pc(0), debug(false) {
    memset(regs, 0, sizeof(regs));
    memset(memory, 0, sizeof(memory));


    // Initialize graphics memory
    memset(graphics.tileMap, 0, sizeof(graphics.tileMap));
    memset(graphics.tileData, 0, sizeof(graphics.tileData));
    memset(graphics.colorPalette, 0, sizeof(graphics.colorPalette));
    memset(graphics.frameBuffer, 0, sizeof(graphics.frameBuffer));
    graphics.screenNeedsUpdate = true;
    graphics.graphicsInitialized = false;
    graphics.graphicsMemoryAccessed = false;
}



// Resets the simulator state
void z16sim::reset() {
    memset(regs, 0, sizeof(regs));
    memset(memory, 0, sizeof(memory));
    pc = 0;
    debug = false; // Reset debug status too
    // Reset infinityCheck counts
    //std::fill(std::begin(infinityCheck), std::end(infinityCheck), 0);
}

// Set debug mode
void z16sim::setDebug(bool d) {
    debug = d;
}


bool z16sim::isDebug() const {
    return debug;
}


// Dumps current register values to stdout
void z16sim::dumpRegisters() const {
    std::cout << "--- Registers ---\n";
    for (int i = 0; i < NUM_REGS; ++i) {
        std::cout << "  " << regNames[i] << " (x" << i << "): 0x"
                  << std::setw(4) << std::setfill('0') << std::hex << regs[i]
                  << std::dec << " (" << (int16_t)regs[i] << ")\n";
    }
    std::cout << "-----------------\n";
}

// Loads machine code from a .bin file into memory. No user prompts.
void z16sim::loadMemoryFromFile(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        throw std::runtime_error("Error: Could not open file " + std::string(filename));
    }

    size_t bytesRead = std::fread(memory, 1, MEM_SIZE, file);
    if (std::ferror(file)) {
        std::fclose(file);
        throw std::runtime_error("Error reading file " + std::string(filename));
    }
    std::fclose(file);

}

// Disassembly function (moved into class, signature matches header)
// Fixed pseudo-instruction implementations for z16sim disassemble function

void z16sim::disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize) {
    uint8_t opcode = inst & 0x7;
    char temp_instr_str[64];

    switch(opcode) {
        case 0x0: { // R-type
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            // NOP: ADD x0, x0 (all fields zero)
            if (funct4 == 0x0 && funct3 == 0x0 && rd_rs1 == 0 && rs2 == 0) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "nop");
            }
            // CLR: XOR rd, rd (same register XOR with itself)
            else if (funct3 == 0x6 && rd_rs1 == rs2) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "clr %s", regNames[rd_rs1]);
            }
            // RET: JR x1 (jump register with ra)
            else if (funct3 == 0x0 && funct4 == 0x4 && rs2 == 1) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "ret");
            }
            // CALL: JALR x1, rs2 (jump and link with ra as destination)
            else if (funct3 == 0x0 && funct4 == 0x8 && rd_rs1 == 1) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "call %s", regNames[rs2]);
            }
            // Regular R-type instructions
            else if(funct4 == 0x0 && funct3 == 0x0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "add %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x1 && funct3 == 0x0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sub %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct3 == 0x1)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "slt %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct3 == 0x2)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sltu %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x2)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sll %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "srl %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x8)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sra %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "or %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x5)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "and %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x6)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "xor %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x7)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "mv %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x0 && funct4 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "jr %s", regNames[rs2]);
            else if (funct3 == 0x0 && funct4 == 0x8)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "jalr %s, %s", regNames[rd_rs1], regNames[rs2]);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown R-Type Instruction");
            break;
        }

        case 0x1: { // I-type
            int16_t imm = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (imm & 0x40)
                imm |= 0xFF80; // Sign extend

            if (funct3 == 0x0) {
                // INC: ADDI rd, 1
                if (imm == 1) {
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "inc %s", regNames[rd_rs1]);
                }
                // DEC: ADDI rd, -1
                else if (imm == -1) {
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "dec %s", regNames[rd_rs1]);
                }
                else {
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "addi %s, %d", regNames[rd_rs1], imm);
                }
            }
            else if (funct3 == 0x1)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "slti %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x2) {
                uint16_t usimm = (inst >> 9) & 0x7F;
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sltiu %s, %u", regNames[rd_rs1], usimm);
            }
            else if(funct3 == 0x3) { // Shift immediates
                uint8_t shamt = imm & 0x7;
                uint8_t shift_type_bits = (imm >> 3) & 0x3;
                if (shift_type_bits == 0x1)
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "slli %s, %d", regNames[rd_rs1], shamt);
                else if (shift_type_bits == 0x2)
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "srli %s, %d", regNames[rd_rs1], shamt);
                else if (shift_type_bits == 0x3)
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "srai %s, %d", regNames[rd_rs1], shamt);
                else
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown I-Type Shift Instruction");
            }
            else if (funct3 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "ori %s, %d", regNames[rd_rs1], imm);
            else if (funct3 == 0x5)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "andi %s, %d", regNames[rd_rs1], imm);
            else if(funct3 == 0x6) {
                // NOT: XORI rd, -1 (XOR with all 1s)
                if (imm == -1) {
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "not %s", regNames[rd_rs1]);
                } else {
                    snprintf(temp_instr_str, sizeof(temp_instr_str), "xori %s, %d", regNames[rd_rs1], imm);
                }
            }
            else if (funct3 == 0x7)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "li %s, %d", regNames[rd_rs1], imm);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown I-Type Instruction");
            break;
        }

        case 0x2: { // B-type (branch)
            int16_t offset = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF0;
            offset <<= 1;

            if (funct3 == 0x0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "beq %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x1)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bne %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x2)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bz %s, %d", regNames[rs1], offset);
            else if (funct3 == 0x3)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bnz %s, %d", regNames[rs1], offset);
            else if (funct3 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "blt %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x5)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bge %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x6)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bltu %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x7)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "bgeu %s, %s, %d", regNames[rs1], regNames[rs2], offset);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown B-Type Instruction");
            break;
        }

        case 0x3: { // S-type (Store)
            int16_t offset = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF0;

            if (funct3 == 0x0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sb %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
            else if (funct3 == 0x1)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "sw %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown Store Instruction");
            break;
        }

        case 0x4: { // L-type (Load)
            int16_t offset = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF0;

            if (funct3 == 0x0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "lb %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x1)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "lw %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x4)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "lbu %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown Load Instruction");
            break;
        }

        case 0x5: { // J-type
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9_4 = (inst >> 9) & 0x3F;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t imm3_1 = (inst >> 3) & 0x7;

            int16_t imm = (imm9_4 << 4) | (imm3_1 << 1);

            if (imm & 0x200)
                imm |= 0xFC00;

            if (f == 0) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "j %d", imm);
            } else if (f == 1) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "jal %s, %d", regNames[rd], imm);
            } else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown J-Type Instruction");
            break;
        }

        case 0x6: { // U-Type
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t imm_15_10 = (inst >> 9) & 0x3F;
            uint8_t imm_9_7 = (inst >> 3) & 0x7;
            uint16_t combined_immediate = (imm_15_10 << 3) | imm_9_7;
            uint16_t effective_immediate = combined_immediate << 7;

            if (f == 0)
                snprintf(temp_instr_str, sizeof(temp_instr_str), "lui %s, 0x%X", regNames[rd], effective_immediate);
            else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "auipc %s, 0x%X", regNames[rd], effective_immediate);
            break;
        }

        case 0x7: { // System Type (ECALL)
            uint8_t funct3 = (inst >> 3) & 0x7;
            uint8_t Service = (inst >> 6) & 0x3FF;
            if (funct3 == 0x0) {
                snprintf(temp_instr_str, sizeof(temp_instr_str), "ecall %d", Service);
            } else
                snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown System Instruction");
            break;
        }

        default:
            snprintf(temp_instr_str, sizeof(temp_instr_str), "Unknown opcode 0x%X", opcode);
            break;
    }

    snprintf(buf, bufSize, "0x%04X: %04X    %s", current_pc, inst, temp_instr_str);
}

// Executes a single instruction. Returns true to continue, false to halt.
int z16sim::executeInstruction(uint16_t inst) {
    uint8_t opcode = inst & 0x7;
    bool pcUpdated = false; // flag: if instruction updated PC directly

    switch(opcode) {
        case 0x0: { // R-type
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            if(funct4 == 0x0 && funct3 == 0x0) // add
                regs[rd_rs1] = regs[rd_rs1] + regs[rs2];
            else if(funct4 == 0x1 && funct3 == 0x0) // sub
                regs[rd_rs1] = regs[rd_rs1] - regs[rs2];
            else if(funct3 == 0x1)    // slt (signed less than)
                regs[rd_rs1] = ((int16_t)regs[rd_rs1] < (int16_t)regs[rs2]) ? 1 :0;
            else if(funct3 == 0x2)    // sltu (unsigned less than)
                regs[rd_rs1] = (regs[rd_rs1] < regs[rs2]) ? 1: 0;
            else if (funct3 == 0x3 && funct4 == 0x2)    // sll (shift left logical)
                regs[rd_rs1] = regs[rd_rs1] << (regs[rs2] & 0xF); // Use lower 4 bits for shift amount (max 15)
            else if (funct3 == 0x3 && funct4 == 0x4)    // srl (shift right logical)
                regs[rd_rs1] = regs[rd_rs1] >> (regs[rs2] & 0xF);
            else if (funct3 == 0x3 && funct4 == 0x8)    // sra (shift right arithmetic)
                regs[rd_rs1] = (uint16_t)(((int16_t)regs[rd_rs1]) >> (regs[rs2] & 0xF));
            else if (funct3 == 0x4)    // or
                regs[rd_rs1] = (regs[rd_rs1] | regs[rs2]);
            else if (funct3 == 0x5)    // and
                regs[rd_rs1] = (regs[rd_rs1] & regs[rs2]);
            else if (funct3 == 0x6)    // xor
                regs[rd_rs1] = (regs[rd_rs1] ^ regs[rs2]);
            else if (funct3 == 0x7)    // mv (move, alias for add x_dest, x_src, x0)
                regs[rd_rs1] = regs[rs2];
            else if (funct3 == 0x0 && funct4 == 0x4) {    // jr (jump register)
                // Infinity check commented out, frontend can implement
                pc = regs[rd_rs1];
                pcUpdated = true;
            }
            else if (funct3 == 0x0 && funct4 == 0x8) { // jalr (jump and link register)
                // Infinity check commented out, frontend can implement
                regs[rd_rs1] = pc + 2; // Store return address
                pc = regs[rs2];       // Jump to target address in rs2
                pcUpdated = true;
            }
            break;
        }
        // case 0x1: { // I-type (addi, slti, etc.)
        //     int16_t imm = (inst >> 9) & 0x7F; // 7-bit immediate
        //     uint8_t rs1_rd = (inst >> 6) & 0x7;
        //     uint8_t funct3 = (inst >> 3) & 0x7;
            
        //     if (imm & 0x40) imm |= 0xFF80; // Sign-extend

        //     uint16_t val1 = this->regs[rs1_rd]; // For I-type, rs1_rd is RS1 or RD

        //     if (funct3 == 0x0) { // addi
        //         this->regs[rs1_rd] = val1 + imm;
        //         this->pc += 2;
        //     } else if (funct3 == 0x1) { // slti (set less than immediate signed)
        //         this->regs[rs1_rd] = ((int16_t)val1 < imm) ? 1 : 0;
        //         this->pc += 2;
        //     } else if (funct3 == 0x2) { // sltui (set less than immediate unsigned)
        //         this->regs[rs1_rd] = (val1 < (uint16_t)imm) ? 1 : 0;
        //         this->pc += 2;
        //     } else if (funct3 == 0x3) { // Shift immediates
        //         uint8_t shift_type = (imm >> 4) & 0x7;
        //         uint8_t shamt = imm & 0xF;
        //         if (shift_type == 0x1) { // slli
        //             this->regs[rs1_rd] = val1 << shamt;
        //             this->pc += 2;
        //         } else if (shift_type == 0x2) { // srli
        //             this->regs[rs1_rd] = val1 >> shamt;
        //             this->pc += 2;
        //         } else if (shift_type == 0x4) { // srai
        //             this->regs[rs1_rd] = (int16_t)val1 >> shamt;
        //             this->pc += 2;
        //         } else {
        //              std::cerr << "Unknown I-type shift instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
        //              return 2;
        //         }
        //     } else if (funct3 == 0x4) { // ori
        //         this->regs[rs1_rd] = val1 | imm;
        //         // this->pc += 2;
        //     } else if (funct3 == 0x5) { // andi
        //         this->regs[rs1_rd] = val1 & imm;
        //         // this->pc += 2;
        //     } else if (funct3 == 0x6) { // xori
        //         this->regs[rs1_rd] = val1 ^ imm;
        //         // this->pc += 2;
        //     } else if (funct3 == 0x7) { // li (load immediate)
        //         this->regs[rs1_rd] = imm;
        //         // this->pc += 2;
        //     } else {
        //         std::cout << "Unknown I-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
        //         return 2;
        //     }
        //     break;
        // }
        case 0x1: { // I-type
            uint8_t imm7   = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7; // Sign extend 7-bit immediate

            if (funct3 == 0x0) // addi
                regs[rd_rs1]+=simm;
            else if(funct3 == 0x1) // slti (set less than immediate signed)
                regs[rd_rs1]=((int16_t)regs[rd_rs1]<simm);
            else if(funct3 == 0x2) // sltiu (set less than immediate unsigned)
                regs[rd_rs1]=(regs[rd_rs1]<(uint16_t)simm); // Note: Comparison with unsigned immediate
            else if(funct3 == 0x3) // Shift immediates
            {
                uint8_t shamt = imm7 & 0x7; // Shift amount (3 bits for 16-bit shift)
                uint8_t shift_type_bits = (imm7 >> 3) & 0x3; // The two most significant bits of the immediate control shift type
                if (shift_type_bits == 0x1) // 01b for SLLI
                    regs[rd_rs1]= regs[rd_rs1] << shamt;
                else if (shift_type_bits == 0x2)// 10b for SRLI
                    regs[rd_rs1]= regs[rd_rs1] >> shamt;
                else if (shift_type_bits == 0x3) // 11b for SRAI
                    regs[rd_rs1]= (uint16_t)(((int16_t)regs[rd_rs1]) >> shamt);
                else {
                    std::cerr << "Unknown I-Type Shift instruction at PC 0x" << std::hex << pc << std::dec << "\n";
                    return 0; // Terminate on unknown instruction
                }
            }
            else if(funct3 == 0x4) // ori
                regs[rd_rs1] = regs[rd_rs1] | simm;
            else if(funct3 == 0x5) // andi
                regs[rd_rs1] = regs[rd_rs1] & simm;
            else if(funct3 == 0x6) // xori
                regs[rd_rs1] = regs[rd_rs1] ^ simm;
            else if(funct3 == 0x7) // li (load immediate)
                regs[rd_rs1] = simm;

            std::cout << "PC: " << pc << std::endl;
            break;
        }
        // Fixed B-type (Branch) instruction handling in executeInstruction
    // case 0x2: { // B-type (Branch)
    //     uint8_t offset_val = (inst >> 12) & 0xF; // 4-bit offset
    //     uint8_t rs1 = (inst >> 6) & 0x7;
    //     uint8_t rs2 = (inst >> 9) & 0x7;
    //     uint8_t funct3 = (inst >> 3) & 0x7;

    //     // Sign extend the 4-bit offset
    //     int16_t simm_offset = (offset_val & 0x8) ? (offset_val | 0xFFF0) : offset_val;
    //     simm_offset <<= 1; // Scale offset by 2 for 16-bit instruction alignment

    //     bool branch_taken = false;
    //     switch (funct3) {
    //         case 0x0: // BEQ
    //             if (regs[rs1] == regs[rs2]) branch_taken = true;
    //             break;
    //         case 0x1: // BNE
    //             if (regs[rs1] != regs[rs2]) branch_taken = true;
    //             break;
    //         case 0x2: // BZ
    //             if (regs[rs1] == 0) branch_taken = true;
    //             break;
    //         case 0x3: // BNZ
    //             if (regs[rs1] != 0) branch_taken = true;
    //             break;
    //         case 0x4: // BLT (signed)
    //             if ((int16_t)regs[rs1] < (int16_t)regs[rs2]) branch_taken = true;
    //             break;
    //         case 0x5: // BGE (signed)
    //             if ((int16_t)regs[rs1] >= (int16_t)regs[rs2]) branch_taken = true;
    //             break;
    //         case 0x6: // BLTU (unsigned)
    //             if (regs[rs1] < regs[rs2]) branch_taken = true;
    //             break;
    //         case 0x7: // BGEU (unsigned)
    //             if (regs[rs1] >= regs[rs2]) branch_taken = true;
    //             break;
    //         default:
    //             std::cerr << "Unknown branch funct3: 0x" << std::hex << (int)funct3 << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
    //             return 0; // Terminate
    //     }

    //     if (branch_taken) {
    //         // Branch offset is relative to current instruction address
    //         // pc currently points to the instruction being executed
    //         uint16_t target_addr = pc + simm_offset;
    //         pc = target_addr;
    //         pcUpdated = true;

    //         // Debug output to verify branch behavior
    //         if (debug) {
    //             std::cout << "Branch taken from 0x" << std::hex << (pc - simm_offset)
    //                     << " to 0x" << pc << std::dec << " (offset: " << simm_offset << ")\n";
    //         }
    //     }
    //     break;
    case 0x2: { // B-type (Branch)
    uint8_t offset_val = (inst >> 12) & 0xF; // 4-bit offset
    uint8_t rs1 = (inst >> 6) & 0x7;
    uint8_t rs2 = (inst >> 9) & 0x7;
    uint8_t funct3 = (inst >> 3) & 0x7;

    // Sign extend the 4-bit offset
    int16_t simm_offset = (offset_val & 0x8) ? (offset_val | 0xFFF0) : offset_val;
    simm_offset <<= 1; // Scale offset by 2 for 16-bit instruction alignment

    bool branch_taken = false;
    switch (funct3) {
        case 0x0: // BEQ
            if (regs[rs1] == regs[rs2]) branch_taken = true;
            break;
        case 0x1: // BNE
            if (regs[rs1] != regs[rs2]) branch_taken = true;
            break;
        case 0x2: // BZ
            if (regs[rs1] == 0) branch_taken = true;
            break;
        case 0x3: // BNZ
            if (regs[rs1] != 0) branch_taken = true;
            break;
        case 0x4: // BLT (signed)
            if ((int16_t)regs[rs1] < (int16_t)regs[rs2]) branch_taken = true;
            break;
        case 0x5: // BGE (signed)
            if ((int16_t)regs[rs1] >= (int16_t)regs[rs2]) branch_taken = true;
            break;
        case 0x6: // BLTU (unsigned)
            if (regs[rs1] < regs[rs2]) branch_taken = true;
            break;
        case 0x7: // BGEU (unsigned)
            if (regs[rs1] >= regs[rs2]) branch_taken = true;
            break;
        default:
            std::cerr << "Unknown branch funct3: 0x" << std::hex << (int)funct3 << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
            return 0; // Terminate
    }

    if (branch_taken) {
        // Branch offset is relative to current instruction address
        // pc currently points to the instruction being executed
        uint16_t target_addr = pc + 2*simm_offset;
        pc = target_addr;
        pcUpdated = true;

        // Debug output to verify branch behavior
        if (debug) {
            std::cout << "Branch taken from 0x" << std::hex << (pc - simm_offset)
                      << " to 0x" << pc << std::dec << " (offset: " << simm_offset << ")\n";
        }
    }
    break;
}
        case 0x3: { // S-type (Store)
            uint8_t offset_val = (inst >> 12) & 0xF;
            uint8_t rs1 = (inst >> 6) & 0x7; // Base register
            uint8_t rs2 = (inst >> 9) & 0x7; // Source register
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm_offset = (offset_val & 0x8) ? (offset_val | 0xFFF0) : offset_val; // Sign extend

            uint16_t effective_address = regs[rs1] + simm_offset;

            // Basic memory bounds check for store
            if (effective_address >= MEM_SIZE || (funct3 == 0x1 && (effective_address + 1 >= MEM_SIZE))) {
                std::cerr << "Error: Memory access out of bounds for store at 0x" << std::hex << effective_address << std::dec << " at PC 0x" << std::hex << pc << std::dec << ".\n";
                return 0; // Terminate simulation
            }

            if (effective_address >= 0xF000 && effective_address <= 0xFA0F) {
                graphics.updateGraphicsMemory(effective_address, regs[rs2] & 0xFF);
            }

            switch (funct3) {
                case 0x0: // sb (store byte)
                    memory[effective_address] = (uint8_t)(regs[rs2] & 0xFF);
                    std::cout << "effective address: " << effective_address << std::endl;
                    if (effective_address >= 0x0000 && effective_address <= 0x093B) {
                    graphics.updateGraphicsMemory(effective_address + 1, (regs[rs2] >> 8) & 0xFF);
                    }

                    break;
                case 0x1: // sw (store word - 16-bit)
                    memory[effective_address] = regs[rs2] & 0xFF;         // Lower byte
                    memory[effective_address + 1] = (regs[rs2] >> 8) & 0xFF; // Upper byte

                    if (effective_address >= 0x0000 && effective_address <= 0x093B) {
                    graphics.updateGraphicsMemory(effective_address + 1, (regs[rs2] >> 8) & 0xFF);
                    }

                    break;
                default:
                    std::cerr << "Unknown store funct3: 0x" << std::hex << (int)funct3 << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
                    return 0; // Terminate
            }
            break;
        }

        case 0x4: { // L-type (Load)
            uint8_t offset_val = (inst >> 12) & 0xF;
            uint8_t rd = (inst >> 6) & 0x7;  // Destination register
            uint8_t rs2 = (inst >> 9) & 0x7; // Base register
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm_offset = (offset_val & 0x8) ? (offset_val | 0xFFF0) : offset_val; // Sign extend

            uint16_t effective_address = regs[rs2] + simm_offset;

            // Basic memory bounds check for load
            if (effective_address >= MEM_SIZE || (funct3 == 0x1 && (effective_address + 1 >= MEM_SIZE))) {
                std::cerr << "Error: Memory access out of bounds for load at 0x" << std::hex << effective_address << std::dec << " at PC 0x" << std::hex << pc << std::dec << ".\n";
                return 0; // Terminate simulation
            }

            switch (funct3) {
                case 0x0: { // lb (load byte signed)
                    int8_t loaded_byte = (int8_t)memory[effective_address];
                    regs[rd] = (uint16_t)loaded_byte; // Sign-extend to 16 bits
                    break;
                }
                case 0x1: { // lw (load word - 16-bit)
                    uint16_t word = memory[effective_address + 1]; // High byte (little-endian assumed)
                    word = (word << 8) | memory[effective_address]; // Low byte
                    regs[rd] = word;
                    break;
                }
                case 0x4: { // lbu (load byte unsigned)
                    regs[rd] = (uint16_t)memory[effective_address]; // Zero-extend to 16 bits
                    break;
                }
                default:
                    std::cerr << "Unknown load funct3: 0x" << std::hex << (int)funct3 << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
                    return 0; // Terminate
            }
            break;
        }
        case 0x5: { // J-type (Jump)
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9_4 = (inst >> 9) & 0x3F;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t imm3_1 = (inst >> 3) & 0x7;

            int16_t simm_jump = (imm9_4 << 4) | (imm3_1 << 1); // Construct 10-bit immediate, scaled

            // Sign-extend the 10-bit immediate
            if (simm_jump & 0x200) // If 10th bit (0x200) is set
                simm_jump |= 0xFC00; // Sign extend to 16 bits

            if (f == 0) { // j (unconditional jump)
                // Infinity check commented out
                pc += simm_jump;
            }
            else { // jal (jump and link)
                // Infinity check commented out
                regs[rd] = pc + 2; // Store return address
                pc += simm_jump;   // Jump to target
            }
            pcUpdated = true;
            break;
        }

        case 0x6: { // U-type (Upper immediate)
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t rd  = (inst >> 6) & 0x7;
            uint16_t I_upper = (inst >> 9) & 0x3F; // 6-bit immediate from bits [14:9]
            uint16_t I_lower = (inst >> 3) & 0x7; // 3 bit immediate
            uint16_t imm_15_7 = (I_upper << 3) | I_lower;
            uint16_t imm = imm_15_7 << 7;


            if (f == 0)    // lui (load upper immediate)
                regs[rd] = imm; 
            else        // auipc (add upper immediate to PC)
                regs[rd] = pc + imm; // Add PC-relative immediate
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
                else
                    printf("Unknown ecall: 0x%03X\n", svc);
                break;
            }
            printf("Invalid system instruction: 0x%X\n", func3);
            break;
        }
        default:
            std::cerr << "Unknown instruction opcode 0x" << std::hex << (int)opcode << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
            return 0; // Terminate on unknown opcode
    }

    if(!pcUpdated)
        pc += 2; // Default: move to next instruction (if PC wasn't updated by a jump/branch)

    return 1; // Continue simulation
}

// Perform one simulation cycle (fetch, decode, execute)
// Returns true if simulation should continue, false if halted (e.g., by ecall 3)
bool z16sim::cycle() {
    if (pc >= MEM_SIZE) {
        std::cerr << "Program Counter out of bounds (0x" << std::hex << pc << std::dec << "). Simulation halted.\n";
        return false; // PC out of bounds, stop simulation
    }

    uint16_t inst = memory[pc] | (memory[pc+1] << 8);

    char disasmBuf[128]; // Buffer for disassembled instruction string
    disassemble(inst, pc, disasmBuf, sizeof(disasmBuf));
    std::cout << disasmBuf << "\n"; // Always print disassembled instruction per cycle

    int exec_result = executeInstruction(inst);

    if (debug) {
        dumpRegisters(); // Dump registers after each instruction if debug is enabled
    }

    if (exec_result == 0) { // executeInstruction returned 0 to signal halt
        return false;
    }

    return true; // Continue to next cycle
}

// void z16sim::initGraphics() {
//     std::cout << "HEREEEEE AT BEGINNING OF INITGRAPHICS" << std::endl;
//     if (graphicsInitialized) return;

//     // Create window
//     window.create(sf::VideoMode(640, 480), "ZX16 Simulator");
//     window.setFramerateLimit(60);
    
//     // Create texture for screen
//     screenTexture.create(320, 240);
//     screenSprite.setTexture(screenTexture);
//     screenSprite.setScale(2.0f, 2.0f); // 2x scale for visibility
    
//     // Initialize default palette
//     colorPalette[0] = 0x00;   // Black
//     colorPalette[1] = 0x1C;   // Red
//     colorPalette[2] = 0xE0;   // Green
//     colorPalette[3] = 0xFC;   // Yellow
//     colorPalette[4] = 0x03;   // Blue
//     colorPalette[5] = 0x1F;   // Magenta
//     colorPalette[6] = 0xE3;  // Cyan
//     colorPalette[7] = 0xFF;   // White
    
//     graphicsInitialized = true;

//     std::cout << "SFML Graphics initialized: 320x240 display (scaled 2x)" << std::endl;
// }

// void z16sim::updateGraphicsMemory(uint16_t addr, uint8_t value) {
//     std::cout << "HEEEREE AT BEGINNING OF UPDATEGRAPHICSMEMORY" << std::endl;
//     if (!graphicsMemoryAccessed) {
//         graphicsMemoryAccessed = true;
//         std::cout << "haallloooooo" << std::endl;
//         initGraphics();
//     }
//         // if (addr >= 0xF000 && addr <= 0xF12B) {
//         if (addr >= 0x0000 && addr <= 0x012B) {
//         // Tile map update
//         // tileMap[addr - 0xF000] = value;
//         std::cout << "tile map updateee" << std::endl;
//         // tileMap[addr] = value;
//         tileMap[addr - 0x0000] = value; 
//         screenNeedsUpdate = true;
//     }



//     // else if (addr >= 0xF200 && addr <= 0xF9FF) {
//     else if (addr >= 0x012C && addr <= 0x092B) {
//         // Tile data update
//         std::cout << "tile data updateee" << std::endl;
//         // int tileIndex = (addr - 0xF200) / 128;
//         // int byteOffset = (addr - 0xF200) % 128;
//         int tileIndex = (addr - 0x012C) / 128;
//         int byteOffset = (addr - 0x012C) % 128;
//         if (tileIndex < 16) {
//             tileData[tileIndex][byteOffset] = value;
//             screenNeedsUpdate = true;
//         }
//     }
//     // else if (addr >= 0xFA00 && addr <= 0xFA0F) {
//     else if (addr >= 0x092C && addr <= 0x093B) {
//         // Color palette update
//         std::cout << "color palette updateee" << std::endl;
//         // colorPalette[addr - 0xFA00] = value;
//         colorPalette[addr - 0x092C] = value;
//         screenNeedsUpdate = true;
//     }
// }


// sf::Color z16sim::paletteToColor(uint8_t colorIndex) {
//     if (colorIndex >= 16) colorIndex = 0;
    
//     uint8_t colorByte = colorPalette[colorIndex];
//     uint8_t r = ((colorByte >> 5) & 0x7) * 36;  // 3 bits -> 0-255
//     uint8_t g = ((colorByte >> 2) & 0x7) * 36;  // 3 bits -> 0-255
//     uint8_t b = (colorByte & 0x3) * 85;         // 2 bits -> 0-255
    
//     return sf::Color(r, g, b);
// }

// void z16sim::renderTile(int tileIndex, int screenX, int screenY) {
//     if (tileIndex >= 16) return;
    
//     for (int y = 0; y < 16; y++) {
//         for (int x = 0; x < 16; x += 2) {
//             int byteIndex = y * 8 + x / 2;
//             uint8_t pixelPair = tileData[tileIndex][byteIndex];
            
//             uint8_t pixel0Color = pixelPair & 0x0F;
//             uint8_t pixel1Color = (pixelPair >> 4) & 0x0F;
            
//             sf::Color color0 = paletteToColor(pixel0Color);
//             sf::Color color1 = paletteToColor(pixel1Color);
            
//             // Set pixels in frame buffer
//             int fbIndex0 = ((screenY + y) * 320 + (screenX + x)) * 4;
//             int fbIndex1 = ((screenY + y) * 320 + (screenX + x + 1)) * 4;
            
//             if (fbIndex0 < 320 * 240 * 4) {
//                 frameBuffer[fbIndex0] = color0.r;
//                 frameBuffer[fbIndex0 + 1] = color0.g;
//                 frameBuffer[fbIndex0 + 2] = color0.b;
//                 frameBuffer[fbIndex0 + 3] = 255;
//             }
            
//             if (fbIndex1 < 320 * 240 * 4) {
//                 frameBuffer[fbIndex1] = color1.r;
//                 frameBuffer[fbIndex1 + 1] = color1.g;
//                 frameBuffer[fbIndex1 + 2] = color1.b;
//                 frameBuffer[fbIndex1 + 3] = 255;
//             }
//         }
//     }
// }

// void z16sim::renderScreen() {
//     if (!screenNeedsUpdate) return;
    
//     // Clear frame buffer
//     memset(frameBuffer, 0, sizeof(frameBuffer));
    
//     // Render each tile position
//     for (int row = 0; row < 15; row++) {
//         for (int col = 0; col < 20; col++) {
//             int tileMapIndex = row * 20 + col;
//             uint8_t tileIndex = tileMap[tileMapIndex];
//             renderTile(tileIndex, col * 16, row * 16);
//         }
//     }
    
//     // Update SFML texture
//     screenTexture.update(frameBuffer);
//     screenNeedsUpdate = false;
// }

// bool z16sim::handleEvents() {
//     sf::Event event;
//     while (window.pollEvent(event)) {
//         if (event.type == sf::Event::Closed) {
//             return false;
//         }
//         if (event.type == sf::Event::KeyPressed) {
//             if (event.key.code == sf::Keyboard::Escape) {
//                 return false;
//             }
//         }
//     }
//     return true;
// }

// void z16sim::cleanup() {
//     if (window.isOpen()) {
//         window.close();
//     }
// }

// bool z16sim::needsGraphics() const {
//     std::cout << "i am in needsGraphics" << std::endl;
//     std::cout << "graphicsMemoryAccessed: " << graphicsMemoryAccessed << std::endl;
//     return graphicsMemoryAccessed;
// }

int main(int argc, char **argv) {
    z16sim simulator; // Create an instance of your simulator
    bool interactive_mode = false;
    std::string machine_code_file;

    // --- 1. Parse command-line arguments ---
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <machine_code_file>\n";
        std::cerr << "       " << argv[0] << " -i <machine_code_file> (for interactive/step-by-step mode)\n";
        return 1;
    }

    if (argc == 3) {
        if (std::string(argv[1]) == "-i") {
            interactive_mode = true;
            machine_code_file = argv[2];
        } else {
            std::cerr << "Unknown option: " << argv[1] << "\n";
            std::cerr << "Usage: " << argv[0] << " <machine_code_file>\n";
            std::cerr << "       " << argv[0] << " -i <machine_code_file> (for interactive/step-by-step mode)\n";
            return 1;
        }
    } else { // argc == 2
        machine_code_file = argv[1];
    }

    // --- 2. Load Memory ---
    try {
        simulator.loadMemoryFromFile(machine_code_file.c_str());
        // This message will be part of the initial output captured by the backend
        std::cout << "Loaded machine code from " << machine_code_file << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // --- 3. Handle Simulation Modes ---
    if (!interactive_mode) {
        // --- Full Simulation Mode ---
        // In this mode, the simulator runs until completion or halt.
        // The `simulator.cycle()` method should print any desired output (disassembly, debug info)
        // based on its internal flags.
        std::cout << "Starting full simulation..." << std::endl;
        simulator.setDebug(true); // Enable debug output for full simulation too
        bool programRunning = true;

        while (programRunning) {
            if (simulator.graphics.needsGraphics()){
                std::cout << "if you see this then needsGraphics returned true" << std::endl;
                if (!simulator.graphics.handleEvents()) break;
                else std::cout << "if you see this then handleEvents returned true" << std::endl;
            }

            if (!simulator.cycle()) {
                programRunning = false;
                std::cout << "Program execution completed." << std::endl;

                if (simulator.graphics.needsGraphics()) {
                        std::cout << "Press ESC or close window to exit." << std::endl;

                        while (simulator.graphics.window.isOpen()) {
                            if (!simulator.graphics.handleEvents()) break;
                            // std::cout << "in heeeereeee" << std::endl;
                            simulator.graphics.renderScreen();
                            simulator.graphics.window.clear();
                            simulator.graphics.window.draw(simulator.graphics.screenSprite);
                            simulator.graphics.window.display();
                            sf::sleep(sf::milliseconds(100));
                        }
                    }
                break;
    
            }

            // Update graphics
            if (simulator.graphics.needsGraphics()) {
                    simulator.graphics.renderScreen();
                    simulator.graphics.window.clear();
                    simulator.graphics.window.draw(simulator.graphics.screenSprite);
                    simulator.graphics.window.display();
                    sf::sleep(sf::milliseconds(16)); // ~60 FPS
                }
        }

            // while(simulator.cycle()) {
            //     // Loop continues as long as simulator.cycle() returns true (not halted)
            // }
            // std::cout << "Full simulation finished." << std::endl;

        } 
    else {
        // --- Interactive/Step-by-Step Mode ---
        // In this mode, the simulator executes one instruction per input from the backend.
        // `setDebug(true)` is crucial here to ensure `cycle()` prints register/memory state.
        simulator.setDebug(true);

        std::cout << "Starting interactive simulation." << std::endl;
        simulator.dumpRegisters(); // Print initial state of registers

        // After initial setup and printing the initial state, signal readiness
        std::cout << "READY_FOR_STEP" << std::endl;
        std::cout.flush(); // Ensure this is sent immediately

        std::string line_input;
        while(true) {
            // Read input from stdin. This will block until the backend sends a newline or 'q'.
            if (!std::getline(std::cin, line_input)) {
                // If getline fails (e.g., stdin is closed by the backend), exit
                std::cerr << "Input stream closed unexpectedly. Exiting interactive mode." << std::endl;
                break;
            }

            // Check if the backend sent a 'q' command to quit the simulation
            if (line_input == "q" || line_input == "Q") {
                std::cout << "Quitting interactive simulation as requested." << std::endl;
                break;
            }


            if (simulator.graphics.needsGraphics() && !simulator.graphics.handleEvents()) break;

            // Execute one simulation cycle (one instruction)
            // The simulator.cycle() method, because debug is true, should print
            // the disassembled instruction and the updated register state.
            if (!simulator.cycle()) {
                // If cycle() returns false, it means the simulation has halted (e.g., reached HLT instruction).
                std::cout << "Simulation halted." << std::endl;
                // No need to send READY_FOR_STEP if the simulation has finished;
                // the process will terminate, and the backend will detect it.
                break;
            }

            // update graphics
            if (simulator.graphics.needsGraphics()) {
                simulator.graphics.renderScreen();
                simulator.graphics.window.clear();
                simulator.graphics.window.draw(simulator.graphics.screenSprite);
                simulator.graphics.window.display();
            }

            // If the simulation is still running, signal readiness for the next step
            std::cout << "READY_FOR_STEP" << std::endl;
            std::cout.flush(); // Ensure this is sent immediately
        }
        std::cout << "Interactive simulation finished." << std::endl;
    }

    simulator.graphics.cleanup();
    return 0;
}