#include "z16sim.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <cctype>




const char* z16sim::regNames[z16sim::NUM_REGS] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};

// z16sim Constructor
z16sim::z16sim() {
    std::memset(this->memory, 0, z16sim::MEM_SIZE);
    std::memset(this->regs, 0, sizeof(this->regs));
    this->pc = 0x0000;
    this->debug = false;
    initializeRegisterMap();
}

int z16sim::getRegisterIndex(const std::string& regName) {
    auto it = regMap.find(regName);
    if (it != regMap.end()) {
        return it->second;
    }
    return -1; // Invalid register
}

void z16sim::initializeRegisterMap() {
    for (int i = 0; i < NUM_REGS; ++i) {
        regMap[regNames[i]] = i;
        regMap["x" + std::to_string(i)] = i;
    }
}



// dumpRegisters method definition
void z16sim::dumpRegisters() const {
    for (int i = 0; i < z16sim::NUM_REGS; ++i) {
        std::cout << regNames[i] << ": 0x"
                  << std::hex << std::setw(4) << std::setfill('0')
                  << regs[i] << std::endl;
    }
}

// loadMemoryFromFile method definition
void z16sim::loadMemoryFromFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1); // Exit if file cannot be opened
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > z16sim::MEM_SIZE) {
        std::cerr << "Warning: File size (" << size << " bytes) exceeds memory size (" << z16sim::MEM_SIZE << " bytes). Only loading " << z16sim::MEM_SIZE << " bytes." << std::endl;
        size = z16sim::MEM_SIZE;
    }

    file.read(reinterpret_cast<char*>(this->memory), size);
    if (!file && file.gcount() != size) { // Check for read errors or incomplete read
        std::cerr << "Error: Failed to read " << size << " bytes from " << filename << " completely. Read " << file.gcount() << " bytes." << std::endl;
        exit(1);
    }
    std::cout << "Loaded " << file.gcount() << " bytes from " << filename << " into memory." << std::endl;
    this->pc = 0; // Initialize PC to 0 after loading the program
}

// cycle method definition
bool z16sim::cycle() {
    // Check for PC out of bounds before fetching instruction
    if (this->pc >= z16sim::MEM_SIZE - 1) { // -1 because 16-bit instructions need 2 bytes
        std::cerr << "Error: Program Counter out of bounds (0x" << std::hex << this->pc << ") at end of memory." << std::endl;
        return false; // Stop simulation
    }

    uint16_t instruction = (this->memory[this->pc + 1] << 8) | this->memory[this->pc];

    char disasm_buf[256];

    z16sim::disassemble(instruction, this->pc, disasm_buf, sizeof(disasm_buf));

    std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc
              << " | Inst: 0x" << std::setw(4) << std::setfill('0') << instruction
              << " | " << disasm_buf << std::endl;

    // Execute the instruction and check its return status
    int status = z16sim::executeInstruction(instruction);

    if (status == 0) {
        return true; // Continue simulation
    }
    else {
        return false; // Stop simulation
    }
}

// executeInstruction method definition
int z16sim::executeInstruction(uint16_t inst) {
    uint8_t opcode = inst & 0x7; // Opcode is bits [2:0]
    uint8_t rs1_rd = (inst >> 6) & 0x7; // For R-type: RD, for B/S/L-type: RS1/RD
    uint8_t rs2 = (inst >> 9) & 0x7; // For R-type: RS2, for B/S/L-type: RS2/Base
    uint8_t funct3 = (inst >> 3) & 0x7;

    int16_t imm; // Signed immediate for I, S, L, J, U types

    switch(opcode) {
        case 0x0: { // R-type (add, sub, slt, sll, etc.)
            uint8_t funct4 = (inst >> 12) & 0xF;
            // For R-type, rs1_rd is RD (destination), rs2 is RS2
            uint16_t val1 = this->regs[rs1_rd]; // Value of RS1 (source for most R-types)
            uint16_t val2 = this->regs[rs2];    // Value of RS2

            if (funct4 == 0x0 && funct3 == 0x0) { // add
                this->regs[rs1_rd] = val1 + val2;
                this->pc += 2; // Advance PC for sequential instruction
            }
            else if (funct4 == 0x1 && funct3 == 0x0) { // sub
                this->regs[rs1_rd] = val1 - val2;
                this->pc += 2;
            }
            else if (funct4 == 0x2 && funct3 == 0x1) { // slt (set less than signed)
                this->regs[rs1_rd] = ((int16_t)val1 < (int16_t)val2) ? 1 : 0;
                this->pc += 2;
            }
            else if (funct4 == 0x3 && funct3 == 0x2) { // sltu (set less than unsigned)
                this->regs[rs1_rd] = (val1 < val2) ? 1 : 0;
                this->pc += 2;
            }
            else if (funct4 == 0x4 && funct3 == 0x3) { // sll (shift left logical)
                this->regs[rs1_rd] = val1 << (val2 & 0xF); // Only use lower 4 bits for shift amount
                this->pc += 2;
            }
            else if (funct4 == 0x5 && funct3 == 0x3) { // srl (shift right logical)
                this->regs[rs1_rd] = val1 >> (val2 & 0xF);
                this->pc += 2;
            }
            else if (funct4 == 0x6 && funct3 == 0x3) { // sra (shift right arithmetic)
                this->regs[rs1_rd] = (int16_t)val1 >> (val2 & 0xF);
                this->pc += 2;
            }
            else if (funct4 == 0x7 && funct3 == 0x4) { // or
                this->regs[rs1_rd] = val1 | val2;
                this->pc += 2;
            }
            else if (funct4 == 0x8 && funct3 == 0x5) { // and
                this->regs[rs1_rd] = val1 & val2;
                this->pc += 2;
            }
            else if (funct4 == 0x9 && funct3 == 0x6) { // xor
                this->regs[rs1_rd] = val1 ^ val2;
                this->pc += 2;
            }
            else if (funct4 == 0xA && funct3 == 0x7) { // mv (move)
                this->regs[rs1_rd] = this->regs[rs2]; // rs1_rd is RD, rs2 is RS
                this->pc += 2;
            }
            else if (funct4 == 0xB && funct3 == 0x0) { // jr (jump register)
                this->pc = this->regs[rs1_rd]; // Jump to address in rs1_rd
                return 0;
            }
            else if (funct4 == 0xC && funct3 == 0x0) { // jalr (jump and link register)
                // this->regs[z16sim::RA_REG] = this->pc + 2; // Store return address
                // this->pc = this->regs[rs1_rd]; // Jump to address in rs1_rd
                uint8_t rd = rs1_rd;
                uint8_t rs2 = (inst >> 9) & 0x7;
                this->regs[rd] = this->pc + 2;
                this->pc = this->regs[rs2];
                return 0;
            }
            else {
                std::cerr << "Unknown R-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2; // Unknown instruction error
            }
            break;
        }

        case 0x1: { // I-type (addi, slti, etc.)
            imm = (inst >> 9) & 0x7F; // 7-bit immediate
            if (imm & 0x40) imm |= 0xFF80; // Sign-extend

            uint16_t val1 = this->regs[rs1_rd]; // For I-type, rs1_rd is RS1 or RD

            if (funct3 == 0x0) { // addi
                this->regs[rs1_rd] = val1 + imm;
                this->pc += 2;
            } else if (funct3 == 0x1) { // slti (set less than immediate signed)
                this->regs[rs1_rd] = ((int16_t)val1 < imm) ? 1 : 0;
                this->pc += 2;
            } else if (funct3 == 0x2) { // sltui (set less than immediate unsigned)
                this->regs[rs1_rd] = (val1 < (uint16_t)imm) ? 1 : 0;
                this->pc += 2;
            } else if (funct3 == 0x3) { // Shift immediates
                uint8_t shift_type = (imm >> 4) & 0x7;
                uint8_t shamt = imm & 0xF;
                if (shift_type == 0x1) { // slli
                    this->regs[rs1_rd] = val1 << shamt;
                    this->pc += 2;
                } else if (shift_type == 0x2) { // srli
                    this->regs[rs1_rd] = val1 >> shamt;
                    this->pc += 2;
                } else if (shift_type == 0x4) { // srai
                    this->regs[rs1_rd] = (int16_t)val1 >> shamt;
                    this->pc += 2;
                } else {
                     std::cerr << "Unknown I-type shift instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                     return 2;
                }
            } else if (funct3 == 0x4) { // ori
                this->regs[rs1_rd] = val1 | imm;
                this->pc += 2;
            } else if (funct3 == 0x5) { // andi
                this->regs[rs1_rd] = val1 & imm;
                this->pc += 2;
            } else if (funct3 == 0x6) { // xori
                this->regs[rs1_rd] = val1 ^ imm;
                this->pc += 2;
            } else if (funct3 == 0x7) { // li (load immediate)
                this->regs[rs1_rd] = imm;
                this->pc += 2;
            } else {
                std::cerr << "Unknown I-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t imm_high = (inst >> 12) & 0xF; // imm[4:1]
            // rs1_rd is RS1, rs2 is RS2
            int16_t offset = (imm_high << 1); // Reconstruct 5-bit signed offset
            if (offset & 0x10) offset |= 0xFFE0; // Sign extend

            // uint16_t target_addr = this->pc + offset;
            uint16_t target_addr = this->pc + 2 + (offset * 2);

            bool branch_taken = false;
            if (funct3 == 0x0) { // beq
                if (this->regs[rs1_rd] == this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x1) { // bne
                if (this->regs[rs1_rd] != this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x2) { // bz (branch if zero) - rs2 is ignored
                if (this->regs[rs1_rd] == 0) branch_taken = true;
            } else if (funct3 == 0x3) { // bnz (branch if not zero) - rs2 is ignored
                if (this->regs[rs1_rd] != 0) branch_taken = true;
            } else if (funct3 == 0x4) { // blt (signed)
                if ((int16_t)this->regs[rs1_rd] < (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x5) { // bge (signed)
                if ((int16_t)this->regs[rs1_rd] >= (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x6) { // bltu (unsigned)
                if (this->regs[rs1_rd] < this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x7) { // bgeu (unsigned)
                if (this->regs[rs1_rd] >= this->regs[rs2]) branch_taken = true;

            } else {
                std::cerr << "Unknown B-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }

            if (branch_taken) {
                if (!updatePC(target_addr, "Branch")) return 3; // Indicate error if PC update fails
            } else {
                this->pc += 2; // No branch, move to next instruction
            }
            break;
        }

        case 0x3: { // S-type (store)
            uint8_t imm4 = (inst >> 12) & 0xF;
            imm = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4; // Sign extend 4-bit immediate
            uint16_t base_addr = this->regs[rs1_rd]; // Base register
            uint16_t data_val = this->regs[rs2];     // Data to store
            uint16_t mem_addr = base_addr + imm;

            if (mem_addr >= z16sim::MEM_SIZE) {
                std::cerr << "Memory access out of bounds for store at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                return 4; // Memory access error
            }

            if (funct3 == 0x0) { // sb (store byte)
                this->memory[mem_addr] = (unsigned char)(data_val & 0xFF);
            } else if (funct3 == 0x1) { // sw (store word)
                if (mem_addr + 1 >= z16sim::MEM_SIZE) { // Check bounds for second byte
                     std::cerr << "Memory access out of bounds for word store at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                     return 4;
                }
                this->memory[mem_addr] = (unsigned char)(data_val & 0xFF);
                this->memory[mem_addr + 1] = (unsigned char)((data_val >> 8) & 0xFF);
            } else {
                std::cerr << "Unknown S-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            this->pc += 2;
            break;
        }

        case 0x4: { // L-type (load)
            uint8_t imm4 = (inst >> 12) & 0xF;
            imm = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4; // Sign extend 4-bit immediate
            uint16_t base_addr = this->regs[rs2]; // Base register
            uint16_t dest_reg = rs1_rd;           // Destination register
            uint16_t mem_addr = base_addr + imm;

            if (mem_addr >= z16sim::MEM_SIZE) {
                std::cerr << "Memory access out of bounds for load at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                return 4; // Memory access error
            }

            if (funct3 == 0x0) { // lb (load byte signed)
                this->regs[dest_reg] = (int8_t)this->memory[mem_addr]; // Sign-extend byte to 16-bit
            } else if (funct3 == 0x1) { // lw (load word)
                 if (mem_addr + 1 >= z16sim::MEM_SIZE) {
                     std::cerr << "Memory access out of bounds for word load at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                     return 4;
                }
                this->regs[dest_reg] = (this->memory[mem_addr + 1] << 8) | this->memory[mem_addr];
            } else if (funct3 == 0x4) { // lbu (load byte unsigned)
                this->regs[dest_reg] = this->memory[mem_addr]; // Zero-extend byte to 16-bit
            } else {
                std::cerr << "Unknown L-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            this->pc += 2;
            break;
        }

        case 0x5: { // J-type (j, jal)
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9to4 = (inst >> 9) & 0x3F;
            uint8_t imm3to1 = (inst >> 3) & 0x7;
            uint8_t rd = (inst >> 6) & 0x7;

            // imm = (imm9to4 << 4) | (imm3to1 << 1); // Reconstruct 9-bit immediate (multiplied by 2)
            // if (imm & 0x200) { // Check if bit 9 (sign bit) is set
            //      imm |= 0xFC00; // Sign extend to 16-bit
            // }
            //
            // uint16_t target_addr = this->pc + imm;

            imm = ((imm9to4 << 4) | (imm3to1 << 1)); // imm[9:1] << 1, imm[0] is zero
            if (imm & 0x200) imm |= 0xFC00;

            int16_t signed_offset = (int16_t)imm;
            uint16_t target_addr = this->pc + signed_offset;


            if (f == 0) { // j (jump)
                if (!updatePC(target_addr, "Jump")) return 3;
            } else if (f == 1) { // jal (jump and link)
                this->regs[rd] = this->pc + 2;
                if (!updatePC(target_addr, "JumpAndLink")) return 3;
            } else {
                std::cerr << "Unknown J-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        case 0x6: { // U-Type (lui, auipc)
            uint8_t f = (inst >> 15) & 0x1;
            uint16_t U_imm = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8);

            if (f == 0) { // lui (load upper immediate)
                this->regs[rs1_rd] = U_imm << 8; // rs1_rd is RD
                this->pc += 2;
            } else if (f == 1) { // auipc (add upper immediate to PC)
                this->regs[rs1_rd] = this->pc + (U_imm << 8); // rs1_rd is RD
                this->pc += 2;
            } else {
                std::cerr << "Unknown U-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        case 0x7: { // SYS-Type (ecall)
            uint16_t svc = (inst >> 6) & 0x3FF; // 10-bit system-call number
            uint8_t func3 = (inst >> 3) & 0x7;

            if (func3 == 0x0) { // ecall
                std::cout << "ECALL (Service: 0x" << std::hex << svc << ") encountered. Terminating simulation." << std::endl;
                return 1; // Indicate halt/termination
            } else {
                std::cerr << "Unknown SYS-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        default: {
            std::cerr << "Unknown opcode: 0x" << std::hex << opcode << " at PC: 0x" << this->pc << std::endl;
            return 2; // Unknown instruction error
        }
    }
    return 0;
}

// reset method definition
void z16sim::reset() {
    std::memset(this->memory, 0, z16sim::MEM_SIZE);
    std::memset(this->regs, 0, sizeof(this->regs));
    this->pc = 0;
    this->debug = false;
    std::cout << "Simulator reset." << std::endl;
}

// updatePC method definition
bool z16sim::updatePC(uint16_t new_pc, const char* instruction_name) {
    if (new_pc >= z16sim::MEM_SIZE) {
        std::cerr << "Error: " << instruction_name << " tried to set PC out of bounds to 0x"
                  << std::hex << new_pc << std::endl;
        return false; // Indicates an error or invalid jump
    }
    this->pc = new_pc;
    return true; // Indicates PC updated successfully
}

// disassemble method definition
void z16sim::disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize) {
    uint8_t opcode = inst & 0x7;
    switch(opcode) {
        case 0x0: { // R-type
            uint8_t funct4  = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            if (funct4 == 0x0 && funct3 == 0x0)
                snprintf(buf, bufSize, "add %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]); // Assuming RD = RS1
            else if (funct4 == 0x1 && funct3 == 0x0)
                snprintf(buf, bufSize, "sub %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x2 && funct3 == 0x1)
                snprintf(buf, bufSize, "slt %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x3 && funct3 == 0x2)
                snprintf(buf, bufSize, "sltu %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x4 && funct3 == 0x3)
                snprintf(buf, bufSize, "sll %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x5 && funct3 == 0x3)
                snprintf(buf, bufSize, "srl %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x6 && funct3 == 0x3)
                snprintf(buf, bufSize, "sra %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x7 && funct3 == 0x4)
                snprintf(buf, bufSize, "or %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x8 && funct3 == 0x5)
                snprintf(buf, bufSize, "and %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0x9 && funct3 == 0x6)
                snprintf(buf, bufSize, "xor %s, %s, %s", regNames[rd_rs1], regNames[rd_rs1], regNames[rs2]);
            else if (funct4 == 0xA && funct3 == 0x7) // mv rd, rs
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
            int16_t imm_val = (inst >> 9) & 0x7F;
            if (imm_val & 0x40) imm_val |= 0xFF80; // Sign-extend

            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "addi %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "slti %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "sltui %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x3) {
                uint8_t shift_type = (imm_val >> 4) & 0x7;
                uint8_t shamt = imm_val & 0xF;
                if (shift_type == 0x1)
                    snprintf(buf, bufSize, "slli %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], shamt);
                else if (shift_type == 0x2)
                    snprintf(buf, bufSize, "srli %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], shamt);
                else if (shift_type == 0x4)
                    snprintf(buf, bufSize, "srai %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], shamt);
                else
                    snprintf(buf, bufSize, "unknown shift imm");
            }
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "ori %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x5)
                snprintf(buf, bufSize, "andi %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x6)
                snprintf(buf, bufSize, "xori %s, %s, %d", regNames[rd_rs1], regNames[rd_rs1], imm_val);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "li %s, %d", regNames[rd_rs1], imm_val);
            else
                snprintf(buf, bufSize, "unknown I-type");
            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t imm_high = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            int16_t offset = (imm_high << 1);
            if (offset & 0x10) offset |= 0xFFE0;

            // uint16_t target_addr = current_pc + offset; // Use current_pc here
            uint16_t target_addr = current_pc + 2 + (offset * 2);

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "beq %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "bne %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "bz %s, 0x%04X", regNames[rs1], target_addr); // rs2 ignored
            else if (funct3 == 0x3)
                snprintf(buf, bufSize, "bnz %s, 0x%04X", regNames[rs1], target_addr); // rs2 ignored
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "blt %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else if (funct3 == 0x5)
                snprintf(buf, bufSize, "bge %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else if (funct3 == 0x6)
                snprintf(buf, bufSize, "bltu %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "bgeu %s, %s, 0x%04X", regNames[rs1], regNames[rs2], target_addr);
            else
                snprintf(buf, bufSize, "unknown B-type");

            break;
        }

        case 0x3: { // S-type (store)
            uint8_t imm4 = (inst >> 12) & 0xF;
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "sb %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "sw %s, %d(%s)", regNames[rs2], offset, regNames[rs1]);
            else
                snprintf(buf, bufSize, "unknown S-type");
            break;
        }

        case 0x4: { // L-type (load)
            uint8_t imm4 = (inst >> 12) & 0xF;
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "lb %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "lw %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "lbu %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else
                snprintf(buf, bufSize, "unknown L-type");
            break;
        }

        case 0x5: { // J-Type (j, jal)
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9to4 = (inst >> 9) & 0x3F;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t imm3to1 = (inst >> 3) & 0x7;

            // int16_t imm_val = (imm9to4 << 4) | (imm3to1 << 1);
            // if (imm_val & 0x200) imm_val |= 0xFC00;
            //
            // uint16_t target_addr = current_pc + imm_val;

            int16_t imm = ((imm9to4 << 4) | (imm3to1 << 1)); // imm[9:1] << 1, imm[0] is zero
            if (imm & 0x200) imm |= 0xFC00; // Proper sign-extension for 10-bit

            int16_t signed_offset = (int16_t)imm; // Interpret as signed
            uint16_t target_addr = this->pc + signed_offset;


            if (f == 0)
                snprintf(buf, bufSize, "j 0x%04X", target_addr);
            else if (f == 1)
                snprintf(buf, bufSize, "jal %s, 0x%04X", regNames[rd], target_addr);
            else
                 snprintf(buf, bufSize, "unknown J-type");
            break;
        }

        case 0x6: { // U-Type
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t rd  = (inst >> 6) & 0x7;
            uint16_t I_val  = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8);
            if(f==0)
                snprintf(buf, bufSize, "lui %s, 0x%X", regNames[rd], I_val);
            else
                snprintf(buf, bufSize, "auipc %s, 0x%X", regNames[rd], I_val);
            break;
        }

        case 0x7: { // SYS-Type
            uint16_t svc = (inst >> 6) & 0x3FF;
            uint8_t func3 = (inst >> 3) & 0x7;

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


void printUsage(const char* progName) {
    std::cerr << "Usage: " << progName << " [-i] <machine_code_file_name.bin>" << std::endl;
    std::cerr << "  -i: Interactive mode (single-stepping)" << std::endl;
}

// int main(int argc, char* argv[]) {
//     bool interactive = false;
//     const char* filename = nullptr;
//
//     // Parse command line arguments
//     if (argc == 2) {
//         // Normal mode: just filename
//         filename = argv[1];
//     } else if (argc == 3) {
//         // Check if first argument is -i
//         if (std::string(argv[1]) == "-i") {
//             interactive = true;
//             filename = argv[2];
//         } else {
//             printUsage(argv[0]);
//             return 1;
//         }
//     } else {
//         printUsage(argv[0]);
//         return 1;
//     }
//
//     z16sim simulator; // Create an instance of the simulator
//
//     // Load the machine code binary from the specified file
//     simulator.loadMemoryFromFile(filename);
//
//     if (interactive) {
//         std::cout << "Interactive mode enabled. Press ENTER to execute next instruction, 'q' then ENTER to quit." << std::endl;
//         std::cout << "Initial state:" << std::endl;
//         simulator.dumpRegisters();
//         std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
//                   << simulator.getPC() << std::endl;
//         std::cout << std::endl;
//
//         // Interactive simulation
//         while (true) {
//             std::cout << "--- Press ENTER to continue (q then ENTER to quit): ";
//             std::cout.flush();
//
//             std::string line;
//             std::getline(std::cin, line); // Read the whole line
//
//             if (line == "q" || line == "Q") {
//                 std::cout << "Simulation terminated by user." << std::endl;
//                 break;
//             }
//
//             // Execute one instruction
//             if (!simulator.cycle()) {
//                 std::cout << "Simulation terminated by instruction." << std::endl;
//                 break;
//             }
//
//             // Dump register state
//             simulator.dumpRegisters();
//             std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
//                       << simulator.getPC() << std::endl;
//             std::cout << std::endl;
//         }
//     } else {
//         // Normal simulation mode
//         while (simulator.cycle()) {
//             // Continue simulation as long as cycle() returns true
//         }
//     }
//
//     // Final register state
//     std::cout << "\n--- Final State ---" << std::endl;
//     simulator.dumpRegisters();
//     std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
//               << simulator.getPC() << std::endl;
//     std::cout << "---------------------\n" << std::endl;
//
//     std::cout << "Simulation finished." << std::endl;
//     return 0;
// }

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
    // Window
    sf::RenderWindow window(sf::VideoMode(800, 600), "ZX16 Simulator");

    // Sound (no external file needed)
    sf::SoundBuffer buffer;
    sf::Int16 samples[44100];
    for (int i = 0; i < 44100; i++) {
        samples[i] = 30000 * sin(i * 0.1); // 440Hz beep
    }
    buffer.loadFromSamples(samples, 44100, 1, 44100);
    sf::Sound sound(buffer);

    // Main loop
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
                sound.play(); // Test audio
            }
        }

        // Clear screen and draw a red square
        window.clear();
        sf::RectangleShape rect(sf::Vector2f(100, 100));
        rect.setFillColor(sf::Color::Red);
        window.draw(rect);
        window.display();
    }
    return 0;
}
