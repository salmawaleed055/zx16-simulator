/*
 * Z16 Instruction Set Simulator (ISS)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mohamed Shalan
 *
 * This simulator accepts a Z16 binary machine code file (with a .bin extension) and assumes that
 * the first instruction is located at memory address 0x0000. It decodes each 16-bit instruction into a
 * human-readable string and prints it, then executes the instruction by updating registers, memory,
 * or performing I/O via ecall.
 *
 * Supported ecall services:
 *   - ecall 1: Print an integer (value in register a0).
 *   - ecall 5: Print a NULL-terminated string (address in register a0).
 *   - ecall 3: Terminate the simulation.
 *
 * Usage:
 *   z16sim <machine_code_file_name>
 *
 */

#include "z16sim.h"



// constructor
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
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;

            if(funct4 == 0x0 && funct3 == 0x0)
                snprintf(buf, bufSize, "add %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x1 && funct3 == 0x0)
                snprintf(buf, bufSize, "sub %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x2 && funct3 == 0x1)
                snprintf(buf, bufSize, "slt %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x3 && funct3 == 0x2)
                snprintf(buf, bufSize, "sltu %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x4 && funct3 == 0x3)
                snprintf(buf, bufSize, "sll %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x5 && funct3 == 0x3)
                snprintf(buf, bufSize, "srl %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x6 && funct3 == 0x3)
                snprintf(buf, bufSize, "sra %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x7 && funct3 == 0x4)
                snprintf(buf, bufSize, "or %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x8 && funct3 == 0x5)
                snprintf(buf, bufSize, "and %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x9 && funct3 == 0x6)
                snprintf(buf, bufSize, "xor %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0xA && funct3 == 0x7)
                snprintf(buf, bufSize, "mv %s, %s", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0xB && funct3 == 0x0)
                snprintf(buf, bufSize, "jr %s", regNames[rd_rs1]); // usually only one register (PC <- rs1)
            else if(funct4 == 0xC && funct3 == 0x0)
                snprintf(buf, bufSize, "jalr %s", regNames[rd_rs1]);
            else
                snprintf(buf, bufSize, "unknown R-type");

            break;
        }
        case 0x1: { // I-type: [15:9] imm[6:0] | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            int16_t imm = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 == 0x0)
                printf("addi %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x1)
                printf("slti %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x2)
                printf("sltui %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x3) {
                // Shift immediate instructions
                uint8_t shamt = imm & 0xF;
                uint8_t shift_type = (imm >> 4) & 0x7;
                if (shift_type == 0x1)
                    printf("slli %s, %d\n", regNames[rd_rs1], shamt);
                else if (shift_type == 0x2)
                    printf("srli %s, %d\n", regNames[rd_rs1], shamt);
                else if (shift_type == 0x4)
                    printf("srai %s, %d\n", regNames[rd_rs1], shamt);
                else
                    printf("unknown shift instruction (funct3 = 0x3)\n");
            }
            else if (funct3 == 0x4)
                printf("ori %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x5)
                printf("andi %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x6)
                printf("xori %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x7)
                printf("li %s, %d\n", regNames[rd_rs1], imm);
            else
                printf("unknown I-type instruction\n");

            break;
        }

        case 0x2: { // B-type (branch): [15:12] offset[4:1] | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
            // your code goes here
            break;
        }

        // complete the rest

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

            else if (funct4 == 0x7 && funct3 == 0x4) // OR
                regs[rd_rs1] = regs[rd_rs1] | regs[rs2];

            else if (funct4 == 0x8 && funct3 == 0x5) // AND
                regs[rd_rs1] = regs[rd_rs1] & regs[rs2];

            else if (funct4 == 0x9 && funct3 == 0x6) // XOR
                regs[rd_rs1] = regs[rd_rs1] ^ regs[rs2];

            else if (funct4 == 0xA && funct3 == 0x7) // MV
                regs[rd_rs1] = regs[rs2];

            else if (funct4 == 0xB && funct3 == 0x0) { // JR
                pc = regs[rd_rs1];
                pcUpdated = 1;
            }

            else if (funct4 == 0xC && funct3 == 0x0) { // JALR
                regs[rd_rs1] = pc + 2;
                pc = regs[rs2];
                pcUpdated = 1;
            }

            break;
        }
        case 0x1: { // I-type: [15:9] imm[6:0] | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            uint8_t imm7   = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7; // sign-extend imm7

            if (funct3 == 0x0) { // ADDI
                regs[rd_rs1] = regs[rd_rs1] + simm;
            }
            else if (funct3 == 0x1) { // SLTI (signed comparison)
                regs[rd_rs1] = ((int16_t)regs[rd_rs1] < simm) ? 1 : 0;
            }
            else if (funct3 == 0x2) { // SLTUI (unsigned comparison)
                regs[rd_rs1] = (regs[rd_rs1] < (uint16_t)simm) ? 1 : 0;
            }
            else if (funct3 == 0x3) {
                uint8_t shamt = imm7 & 0xF; // shift amount
                uint8_t shift_type = (imm7 >> 4) & 0x7; // imm7[6:4]
                if (shift_type == 0x1) { // SLLI
                    regs[rd_rs1] = regs[rd_rs1] << shamt;
                }
                else if (shift_type == 0x2) { // SRLI
                    regs[rd_rs1] = regs[rd_rs1] >> shamt;
                }
                else if (shift_type == 0x4) { // SRAI
                    regs[rd_rs1] = ((int16_t)regs[rd_rs1]) >> shamt;
                }
            }
            else if (funct3 == 0x4) { // ORI
                regs[rd_rs1] = regs[rd_rs1] | simm;
            }
            else if (funct3 == 0x5) { // ANDI
                regs[rd_rs1] = regs[rd_rs1] & simm;
            }
            else if (funct3 == 0x6) { // XORI
                regs[rd_rs1] = regs[rd_rs1] ^ simm;
            }
            else if (funct3 == 0x7) { // LI
                regs[rd_rs1] = simm;
            }

            break;
        }

        case 0x2: { // B-type (branch)

            // your code goes here
            break;
        }
        case 0x3: { // L-type (load/store)
            // your code goes here
            break;
        }
        case 0x5: { // J-type (jump)
            // your code goes here
            break;
        }
        case 0x6: { // U-type
            // your code goes here
            break;
        }
        case 0x7: { // System instruction (ecall)
            // your code goes here
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
// Loads the binary machine code image from the specified file into simulated memory.
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
    // Terminate if PC goes out of bounds
    if(pc >= MEM_SIZE - 1) return 0;

    char disasmBuf[128];

    // Fetch a 16-bit instruction from memory (little-endian)
    uint16_t inst = memory[pc] | (memory[pc+1] << 8);

    disassemble(inst, pc, disasmBuf, sizeof(disasmBuf));
    printf("0x%04X: %04X    %s\n", pc, inst, disasmBuf);

    if(!executeInstruction(inst)) return false;

    return true;
}


