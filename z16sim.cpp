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
    this->pc = 0; // Initialize PC to 0 after loading the program (will be overridden in main)
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
    uint8_t funct3 = (inst >> 3) & 0x7;

    int16_t imm; // Signed immediate for I, S, L, J, U types

    switch(opcode) {
        case 0x0: { // R-type (add, sub, slt, sll, etc.)
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rd = (inst >> 6) & 0x7;    // RD (bits 8:6)
            uint8_t rs1 = (inst >> 9) & 0x7;   // RS1 (bits 11:9)
            uint8_t rs2_r = (inst >> 12) & 0x7; // RS2 for ALU ops (bits 14:12), renamed to avoid conflict

            uint16_t val1 = this->regs[rs1];
            uint16_t val2 = this->regs[rs2_r];

            if (funct4 == 0x0 && funct3 == 0x0) { // add rd, rs1, rs2
                this->regs[rd] = val1 + val2;
                this->pc += 2;
            }
            else if (funct4 == 0x1 && funct3 == 0x0) { // sub rd, rs1, rs2
                this->regs[rd] = val1 - val2;
                this->pc += 2;
            }
            else if (funct4 == 0x2 && funct3 == 0x1) { // slt rd, rs1, rs2 (set less than signed)
                this->regs[rd] = ((int16_t)val1 < (int16_t)val2) ? 1 : 0;
                this->pc += 2;
            }
            else if (funct4 == 0x3 && funct3 == 0x2) { // sltu rd, rs1, rs2 (set less than unsigned)
                this->regs[rd] = (val1 < val2) ? 1 : 0;
                this->pc += 2;
            }
            else if (funct4 == 0x4 && funct3 == 0x3) { // sll rd, rs1, rs2 (shift left logical)
                this->regs[rd] = val1 << (val2 & 0xF); // Only use lower 4 bits for shift amount
                this->pc += 2;
            }
            else if (funct4 == 0x5 && funct3 == 0x3) { // srl rd, rs1, rs2 (shift right logical)
                this->regs[rd] = val1 >> (val2 & 0xF);
                this->pc += 2;
            }
            else if (funct4 == 0x6 && funct3 == 0x3) { // sra rd, rs1, rs2 (shift right arithmetic)
                this->regs[rd] = (int16_t)val1 >> (val2 & 0xF);
                this->pc += 2;
            }
            else if (funct4 == 0x7 && funct3 == 0x4) { // or rd, rs1, rs2
                this->regs[rd] = val1 | val2;
                this->pc += 2;
            }
            else if (funct4 == 0x8 && funct3 == 0x5) { // and rd, rs1, rs2
                this->regs[rd] = val1 & val2;
                this->pc += 2;
            }
            else if (funct4 == 0x9 && funct3 == 0x6) { // xor rd, rs1, rs2
                this->regs[rd] = val1 ^ val2;
                this->pc += 2;
            }
            else if (funct3 == 0x7) { // mv rd, rs1 (uses rs1 for source, rd for dest)
                // Assuming funct4 is 0 for mv for simpler encoding, not used as rs2_r
                uint8_t mv_rd = (inst >> 6) & 0x7; // Destination for mv
                uint8_t mv_rs = (inst >> 9) & 0x7; // Source for mv
                this->regs[mv_rd] = this->regs[mv_rs];
                this->pc += 2;
            }
            else if (funct4 == 0xB && funct3 == 0x0) { // jr rs1
                this->pc = this->regs[rs1]; // Jump to address in rs1
                return 0; // PC is updated directly, no +2
            }
            else if (funct4 == 0xC && funct3 == 0x0) { // jalr rd, rs1
                this->regs[rd] = this->pc + 2; // Store return address in rd
                this->pc = this->regs[rs1];    // Jump to address in rs1
                return 0; // PC is updated directly, no +2
            }
            else {
                std::cerr << "Unknown R-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2; // Unknown instruction error
            }
            break;
        }

        case 0x1: { // I-type (addi, slti, andi, li, etc.)
            imm = (inst >> 9) & 0x7F; // 7-bit immediate from bits 15:9
            if (imm & 0x40) imm |= 0xFF80; // Sign-extend the 7-bit immediate

            uint8_t rd = (inst >> 6) & 0x7;  // RD for I-type (bits 8:6)
            uint8_t rs1 = (inst >> 9) & 0x7; // RS1 for I-type (bits 11:9)
            uint16_t rs1_val = this->regs[rs1];

            if (funct3 == 0x0) { // addi rd, rs1, imm
                this->regs[rd] = rs1_val + imm;
                this->pc += 2;
            } else if (funct3 == 0x1) { // slti rd, rs1, imm (set less than immediate signed)
                this->regs[rd] = ((int16_t)rs1_val < imm) ? 1 : 0;
                this->pc += 2;
            } else if (funct3 == 0x2) { // sltui rd, rs1, imm (set less than immediate unsigned)
                this->regs[rd] = (rs1_val < (uint16_t)imm) ? 1 : 0;
                this->pc += 2;
            } else if (funct3 == 0x3) { // Shift immediates
                uint8_t shift_type = (imm >> 4) & 0x7;
                uint8_t shamt = imm & 0xF; // Assumed shamt is lower 4 bits of imm
                if (shift_type == 0x1) { // slli rd, rs1, shamt
                    this->regs[rd] = rs1_val << shamt;
                    this->pc += 2;
                } else if (shift_type == 0x2) { // srli rd, rs1, shamt
                    this->regs[rd] = rs1_val >> shamt;
                    this->pc += 2;
                } else if (shift_type == 0x4) { // srai rd, rs1, shamt
                    this->regs[rd] = (int16_t)rs1_val >> shamt;
                    this->pc += 2;
                } else {
                     std::cerr << "Unknown I-type shift instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                     return 2;
                }
            } else if (funct3 == 0x4) { // ori rd, rs1, imm
                this->regs[rd] = rs1_val | imm;
                this->pc += 2;
            } else if (funct3 == 0x5) { // andi rd, rs1, imm
                this->regs[rd] = rs1_val & imm;
                this->pc += 2;
            } else if (funct3 == 0x6) { // xori rd, rs1, imm
                this->regs[rd] = rs1_val ^ imm;
                this->pc += 2;
            } else if (funct3 == 0x7) { // li rd, imm (load immediate) - RS1 field is ignored for LI
                this->regs[rd] = imm; // Value is purely the immediate
                this->pc += 2;
            } else {
                std::cerr << "Unknown I-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t imm_high = (inst >> 12) & 0xF; // imm[4:1]
            uint8_t rs2 = (inst >> 9) & 0x7; // RS2
            uint8_t rs1 = (inst >> 6) & 0x7; // RS1

            int16_t offset = (imm_high << 1); // Reconstruct 5-bit signed offset for instructions
            if (offset & 0x10) offset |= 0xFFE0; // Sign extend to 16-bit

            uint16_t target_addr = this->pc + 2 + (offset * 2); // PC + 2 + offset*2 bytes

            bool branch_taken = false;
            if (funct3 == 0x0) { // beq
                if (this->regs[rs1] == this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x1) { // bne
                if (this->regs[rs1] != this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x2) { // bz (branch if zero) - rs2 is ignored
                if (this->regs[rs1] == 0) branch_taken = true;
            } else if (funct3 == 0x3) { // bnz (branch if not zero) - rs2 is ignored
                if (this->regs[rs1] != 0) branch_taken = true;
            } else if (funct3 == 0x4) { // blt (signed)
                if ((int16_t)this->regs[rs1] < (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x5) { // bge (signed)
                if ((int16_t)this->regs[rs1] >= (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x6) { // bltu (unsigned)
                if (this->regs[rs1] < this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x7) { // bgeu (unsigned)
                if (this->regs[rs1] >= this->regs[rs2]) branch_taken = true;
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
            uint8_t rs2_val = (inst >> 9) & 0x7;     // Data to store
            uint8_t rs1_base = (inst >> 6) & 0x7;    // Base register
            uint16_t base_addr = this->regs[rs1_base];
            uint16_t data_val = this->regs[rs2_val];
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
            uint8_t rs1_base = (inst >> 9) & 0x7; // Base register
            uint8_t rd_dest = (inst >> 6) & 0x7; // Destination register
            uint16_t base_addr = this->regs[rs1_base];
            uint16_t mem_addr = base_addr + imm;

            if (mem_addr >= z16sim::MEM_SIZE) {
                std::cerr << "Memory access out of bounds for load at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                return 4; // Memory access error
            }

            if (funct3 == 0x0) { // lb (load byte signed)
                this->regs[rd_dest] = (int8_t)this->memory[mem_addr]; // Sign-extend byte to 16-bit
            } else if (funct3 == 0x1) { // lw (load word)
                 if (mem_addr + 1 >= z16sim::MEM_SIZE) {
                     std::cerr << "Memory access out of bounds for word load at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                     return 4;
                }
                this->regs[rd_dest] = (this->memory[mem_addr + 1] << 8) | this->memory[mem_addr];
            } else if (funct3 == 0x4) { // lbu (load byte unsigned)
                this->regs[rd_dest] = this->memory[mem_addr]; // Zero-extend byte to 16-bit
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

            imm = ((imm9to4 << 4) | (imm3to1 << 1)); // imm[9:1] << 1, imm[0] is zero
            if (imm & 0x200) imm |= 0xFC00; // Proper sign-extension for 10-bit offset

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
            uint8_t rd = (inst >> 6) & 0x7;
            uint16_t U_imm = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8); // This extraction looks unusual for a 10-bit immediate (U-type usually 20-bit or similar)
                                                                       // Recheck Zx16 U-type diagram for U_imm.
                                                                       // Assuming U_imm is from bits 14:3 as it seems to be in your code structure
                                                                       // which is (inst >> 3) & 0x7FF
                                                                       // Let's use the interpretation that bits 14:6 is Imm[8:0] and bits 5:3 is Imm[2:0]
                                                                       // If it's a 10-bit immediate, it would be (inst >> 3) & 0x3FF
                                                                       // Your current `((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8)` is problematic.
                                                                       // Assuming U_imm is bits 14:3
            uint16_t actual_U_imm = (inst >> 3) & 0x7FF; // 11-bit immediate if bits 14:3 is the immediate

            if (f == 0) { // lui (load upper immediate)
                this->regs[rd] = actual_U_imm << 5; // Load upper 11 bits to bits 15:5 (adjust shift based on ISA)
                                                    // Standard lui loads to upper 20 bits, so this Zx16 lui is very different.
                                                    // Assuming it means load immediate to bits 15:5 of RD.
                this->pc += 2;
            } else if (f == 1) { // auipc (add upper immediate to PC)
                this->regs[rd] = this->pc + (actual_U_imm << 5); // Add upper 11 bits to PC
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
            uint8_t funct4_dis  = (inst >> 12) & 0xF; // Renamed to avoid conflicts
            uint8_t rs2_dis     = (inst >> 12) & 0x7; // Bits 14:12 - Second Source (assuming funct4_dis is just higher bits)
            uint8_t rs1_dis     = (inst >> 9) & 0x7;  // Bits 11:9 - First Source
            uint8_t rd_dis      = (inst >> 6) & 0x7;  // Bits 8:6 - Destination Register
            uint8_t funct3      = (inst >> 3) & 0x7;

            if (funct4_dis == 0x0 && funct3 == 0x0)
                snprintf(buf, bufSize, "add %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x1 && funct3 == 0x0)
                snprintf(buf, bufSize, "sub %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x2 && funct3 == 0x1)
                snprintf(buf, bufSize, "slt %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x3 && funct3 == 0x2)
                snprintf(buf, bufSize, "sltu %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x4 && funct3 == 0x3)
                snprintf(buf, bufSize, "sll %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x5 && funct3 == 0x3)
                snprintf(buf, bufSize, "srl %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x6 && funct3 == 0x3)
                snprintf(buf, bufSize, "sra %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x7 && funct3 == 0x4)
                snprintf(buf, bufSize, "or %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x8 && funct3 == 0x5)
                snprintf(buf, bufSize, "and %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct4_dis == 0x9 && funct3 == 0x6)
                snprintf(buf, bufSize, "xor %s, %s, %s", regNames[rd_dis], regNames[rs1_dis], regNames[rs2_dis]);
            else if (funct3 == 0x7) // mv rd, rs1 (assuming funct4 is 0)
                snprintf(buf, bufSize, "mv %s, %s", regNames[rd_dis], regNames[rs1_dis]);
            else if (funct4_dis == 0xB && funct3 == 0x0)
                snprintf(buf, bufSize, "jr %s", regNames[rs1_dis]); // jr rs1
            else if (funct4_dis == 0xC && funct3 == 0x0)
                snprintf(buf, bufSize, "jalr %s, %s", regNames[rd_dis], regNames[rs1_dis]); // jalr rd, rs1
            else
                snprintf(buf, bufSize, "unknown R-type (F4:0x%X F3:0x%X)", funct4_dis, funct3);
            break; // <--- This break statement is CRUCIAL!
        }

        case 0x1: { // I-type
            int16_t imm_val = (inst >> 9) & 0x7F; // 7-bit immediate from bits 15:9
            if (imm_val & 0x40) imm_val |= 0xFF80; // Sign-extend

            uint8_t rd = (inst >> 6) & 0x7;  // RD (bits 8:6)
            uint8_t rs1 = (inst >> 9) & 0x7; // RS1 (bits 11:9)
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "addi %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "slti %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "sltui %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x3) {
                uint8_t shift_type = (imm_val >> 4) & 0x7;
                uint8_t shamt = imm_val & 0xF;
                if (shift_type == 0x1)
                    snprintf(buf, bufSize, "slli %s, %s, %d", regNames[rd], regNames[rs1], shamt);
                else if (shift_type == 0x2)
                    snprintf(buf, bufSize, "srli %s, %s, %d", regNames[rd], regNames[rs1], shamt);
                else if (shift_type == 0x4)
                    snprintf(buf, bufSize, "srai %s, %s, %d", regNames[rd], regNames[rs1], shamt);
                else
                    snprintf(buf, bufSize, "unknown shift imm");
            }
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "ori %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x5)
                snprintf(buf, bufSize, "andi %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x6)
                snprintf(buf, bufSize, "xori %s, %s, %d", regNames[rd], regNames[rs1], imm_val);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "li %s, %d", regNames[rd], imm_val); // li does not use RS1 field explicitly
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
            uint8_t rs2_store = (inst >> 9) & 0x7; // Source register to store
            uint8_t rs1_base = (inst >> 6) & 0x7;  // Base register for address
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "sb %s, %d(%s)", regNames[rs2_store], offset, regNames[rs1_base]);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "sw %s, %d(%s)", regNames[rs2_store], offset, regNames[rs1_base]);
            else
                snprintf(buf, bufSize, "unknown S-type");
            break;
        }

        case 0x4: { // L-type (load)
            uint8_t imm4 = (inst >> 12) & 0xF;
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint8_t rs1_base = (inst >> 9) & 0x7; // Base register for address
            uint8_t rd_load = (inst >> 6) & 0x7;  // Destination register
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                snprintf(buf, bufSize, "lb %s, %d(%s)", regNames[rd_load], offset, regNames[rs1_base]);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "lw %s, %d(%s)", regNames[rd_load], offset, regNames[rs1_base]);
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "lbu %s, %d(%s)", regNames[rd_load], offset, regNames[rs1_base]);
            else
                snprintf(buf, bufSize, "unknown L-type");
            break;
        }

        case 0x5: { // J-Type (j, jal)
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9to4 = (inst >> 9) & 0x3F;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t imm3to1 = (inst >> 3) & 0x7;

            int16_t imm_signed = ((imm9to4 << 4) | (imm3to1 << 1)); // imm[9:1] << 1
            if (imm_signed & 0x200) imm_signed |= 0xFC00; // Sign-extend 10-bit offset

            uint16_t target_addr = current_pc + imm_signed;


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
            uint16_t U_imm_actual = (inst >> 3) & 0x7FF; // 11-bit immediate from bits 14:3

            if(f==0)
                snprintf(buf, bufSize, "lui %s, 0x%X", regNames[rd], U_imm_actual);
            else
                snprintf(buf, bufSize, "auipc %s, 0x%X", regNames[rd], U_imm_actual);
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

int main(int argc, char* argv[]) {
    bool interactive = false;
    const char* filename = nullptr;

    // Parse command line arguments
    if (argc == 2) {
        // Normal mode: just filename
        filename = argv[1];
    } else if (argc == 3) {
        // Check if first argument is -i
        if (std::string(argv[1]) == "-i") {
            interactive = true;
            filename = argv[2];
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } else {
        printUsage(argv[0]);
        return 1;
    }

    z16sim simulator; // Create an instance of the simulator

    // Load the machine code binary from the specified file
    simulator.loadMemoryFromFile(filename);

    // Set the initial PC to where your actual code starts (0x0020 for your example)
    simulator.setPC(0x0020);

    if (interactive) {
        std::cout << "Interactive mode enabled. Press ENTER to execute next instruction, 'q' then ENTER to quit." << std::endl;
        std::cout << "Initial state:" << std::endl;
        simulator.dumpRegisters();
        std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << simulator.getPC() << std::endl;
        std::cout << std::endl;

        // Interactive simulation
        while (true) {
            std::cout << "--- Press ENTER to continue (q then ENTER to quit): ";
            std::cout.flush();

            std::string line;
            std::getline(std::cin, line); // Read the whole line

            if (line == "q" || line == "Q") {
                std::cout << "Simulation terminated by user." << std::endl;
                break;
            }

            // Execute one instruction
            if (!simulator.cycle()) {
                std::cout << "Simulation terminated by instruction." << std::endl;
                break;
            }

            // Dump register state
            simulator.dumpRegisters();
            std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
                      << simulator.getPC() << std::endl;
            std::cout << std::endl;
        }
    } else {
        // Normal simulation mode
        while (simulator.cycle()) {
            // Continue simulation as long as cycle() returns true
        }
    }

    // Final register state
    std::cout << "\n--- Final State ---" << std::endl;
    simulator.dumpRegisters();
    std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
              << simulator.getPC() << std::endl;
    std::cout << "---------------------\n" << std::endl;

    std::cout << "Simulation finished." << std::endl;
    return 0;
}