#include "z16sim.h" // Include your header file
#include <iostream>  // For std::cout, std::cerr
#include <iomanip>   // For std::hex, std::setw, std::setfill
#include <stdexcept> // For std::runtime_error
#include <cstdio>    // For snprintf, fopen, fread, ferror, fclose

// Initialize static member (outside class definition)
const char* z16sim::regNames[z16sim::NUM_REGS] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};


// Constructor
z16sim::z16sim() : pc(0), debug(false) {
    std::memset(regs, 0, sizeof(regs));
    std::memset(memory, 0, sizeof(memory));
    // Initialize infinityCheck if it's a member array
    // If you want to use it, uncomment it and consider it's reset in reset()
    // std::fill(std::begin(infinityCheck), std::end(infinityCheck), 0);
    //
    // For now, I'll comment out the interactive loop detection.
    // If you want it, it's better handled at the calling (main) level,
    // where it can decide if it wants to prompt the user or auto-terminate.
}

// Resets the simulator state
void z16sim::reset() {
    std::memset(regs, 0, sizeof(regs));
    std::memset(memory, 0, sizeof(memory));
    pc = 0;
    debug = false; // Reset debug status too
    // Reset infinityCheck counts
    //std::fill(std::begin(infinityCheck), std::end(infinityCheck), 0);
}

// Set debug mode
void z16sim::setDebug(bool d) {
    debug = d;
}

// Set verbose mode (for messages like "Loaded X bytes")
void z16sim::setVerbose(bool val) {
    // Assuming you add a 'verbose' member to your class, e.g., bool verbose;
    // this->verbose = val;
    // If you don't have it, just remove this function or add the member.
    // For this example, I will assume `verbose` member exists.
}

// Optional getter for debug status
bool z16sim::isDebug() const {
    return debug;
}

// Optional getter for verbose status (assuming it exists)
bool z16sim::isVerbose() const {
    // return verbose;
    return false; // Placeholder if you haven't added 'verbose' member
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

    // If verbose mode is implemented and enabled, then print:
    // if (verbose) { // Assuming 'verbose' is a member variable
    //     std::cout << "Loaded " << bytesRead << " bytes into memory\n";
    // }
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
            break;
        }
        // Fixed B-type (Branch) instruction handling in executeInstruction
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
        uint16_t target_addr = pc + simm_offset;
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

            switch (funct3) {
                case 0x0: // sb (store byte)
                    memory[effective_address] = (uint8_t)(regs[rs2] & 0xFF);
                    break;
                case 0x1: // sw (store word - 16-bit)
                    memory[effective_address] = regs[rs2] & 0xFF;         // Lower byte
                    memory[effective_address + 1] = (regs[rs2] >> 8) & 0xFF; // Upper byte
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
            uint16_t I_upper = (inst >> 7) & 0xFF; // 8-bit immediate from bits [14:7]

            if(f==0)    // lui (load upper immediate)
                regs[rd] = (I_upper << 7); // Load upper 8 bits, shifted by 7 to align
            else        // auipc (add upper immediate to PC)
                regs[rd] = pc + (I_upper << 7); // Add PC-relative immediate
            break;
        }
        case 0x7: { // System instruction (ecall)
            uint8_t funct3  = (inst >> 3) & 0x7;
            uint8_t Service  = (inst >> 6) & 0x7; // Service number (from bits 8:6)
            if(funct3 == 0x0) {
                switch (Service) {
                    case 0x1: { // print_int (a0)
                        int16_t number = regs[6]; // Assuming a0 (x6) holds the integer
                        std::cout << number << "\n";
                        break;
                    }
                    case 0x5: { // print_string (a0)
                        uint16_t string_addr = regs[6]; // Assuming a0 (x6) holds the address
                        if (string_addr < MEM_SIZE) {
                            std::cout << (char*)&memory[string_addr] << "\n";
                        } else {
                            std::cerr << "Error: Attempted to print string from out-of-bounds address 0x" << std::hex << string_addr << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
                            return 0; // Terminate simulation on error
                        }
                        break;
                    }
                    case 0x3: { // exit
                        return 0; // Signal to halt simulation from within the class
                    }
                    default:
                        std::cerr << "Unknown ecall service: " << (int)Service << " at PC 0x" << std::hex << pc << std::dec << "\n";
                        return 0; // Terminate on unknown ecall service
                }
            } else {
                std::cerr << "Unknown system funct3: 0x" << std::hex << (int)funct3 << std::dec << " at PC 0x" << std::hex << pc << std::dec << "\n";
                return 0; // Terminate on unknown system instruction
            }
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
        std::cout << "Loaded machine code from " << machine_code_file << "\n";
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    // --- 3. Handle Simulation Modes ---
    if (!interactive_mode) {
        // --- Full Simulation Mode ---
        // In this mode, the simulator runs until completion or halt.
        // The `simulator.cycle()` method should print any desired output (disassembly, debug info)
        // based on its internal flags.
        std::cout << "Starting full simulation...\n";
        while(simulator.cycle()) {
            // Loop continues as long as simulator.cycle() returns true (not halted)
        }
        std::cout << "Full simulation finished.\n";

    } else {
        // --- Interactive/Step-by-Step Mode ---
        // In this mode, the simulator executes one instruction per input from the backend.
        // `setDebug(true)` is crucial here to ensure `cycle()` prints register/memory state.
        simulator.setDebug(true);

        std::cout << "Starting interactive simulation.\n";
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
                std::cout << "Quitting interactive simulation as requested.\n";
                break;
            }

            // Execute one simulation cycle (one instruction)
            // The simulator.cycle() method, because debug is true, should print
            // the disassembled instruction and the updated register state.
            if (!simulator.cycle()) {
                // If cycle() returns false, it means the simulation has halted (e.g., reached HLT instruction).
                std::cout << "Simulation halted.\n";
                // No need to send READY_FOR_STEP if the simulation has finished;
                // the process will terminate, and the backend will detect it.
                break;
            }

            // If the simulation is still running, signal readiness for the next step
            std::cout << "READY_FOR_STEP" << std::endl;
            std::cout.flush(); // Ensure this is sent immediately
        }
        std::cout << "Interactive simulation finished.\n"; // This might not be seen by backend if process exits immediately after halt

    }

    return 0;
}