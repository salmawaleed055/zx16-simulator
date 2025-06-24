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
        case 0x0: { // R-type: [15:12] funct4 | [11:9] rs2 | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;
            if(funct4 == 0x0 && funct3 == 0x0)
                printf("add %s, %s", regNames[rd_rs1], regNames[rs2]);
            // complete the rest
            break;
        }
        case 0x1: { // I-type: [15:9] imm[6:0] | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            // your code goes here
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
            if(funct4 == 0x0 && funct3 == 0x0) // add
                regs[rd_rs1] = regs[rd_rs1] + regs[rs2];
            else if(funct4 == 0x1 && funct3 == 0x0) // sub
                regs[rd_rs1] = regs[rd_rs1] - regs[rs2];
            // complete the rest
            break;
        }
        case 0x1: { // I-type
            uint8_t imm7   = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7;
            // your code goes here
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


