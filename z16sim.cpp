
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MEM_SIZE 65536  // 64KB memory

// Global simulated memory and register file.
unsigned char memory[MEM_SIZE];
uint16_t regs[8];      // 8 registers (16-bit each): x0, x1, x2, x3, x4, x5, x6, x7
uint16_t pc = 0;       // Program counter (16-bit)

int infinityCheck[]={0,0,0,0};  // counts how many times a jump instruction is called
bool debug=false;       // debugging option

// Register ABI names for display (x0 = t0, x1 = ra, x2 = sp, x3 = s0, x4 = s1, x5 = t1, x6 = a0, x7 = a1)
const char *regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

// -----------------------
// Disassembly Function
// -----------------------
//
// Decodes a 16-bit instruction 'inst' (fetched at address 'pc') and writes a human‑readable
// string to 'buf' (of size bufSize). This decoder uses the opcode (bits [2:0]) to distinguish
// among R‑, I‑, B‑, L‑, J‑, U‑, and System instructions.
void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize) {
    uint8_t opcode = inst & 0x7;
    switch(opcode) {
        case 0x0: { // R-type: [15:12] funct4 | [11:9] rs2 | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;
            if(funct4 == 0x0 && funct3 == 0x0)
                printf("add %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if(funct4 == 0x1 && funct3 == 0x0)
                printf("sub %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if(funct3 == 0x1)
                printf("slt %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if(funct3 == 0x2)
                printf("sltu %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x2)
                printf("sll %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x4)
                printf("srl %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x8)
                printf("sra %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x4)
                printf("or %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x5)
                printf("and %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x6)
                printf("xor %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x7)
                printf("mv %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else if (funct3 == 0x0 && funct4 == 0x4)
                printf("jr %s\n", regNames[rs2]);
            else if (funct3 == 0x0 && funct4 == 0x8)
                printf("jalr %s, %s\n", regNames[rd_rs1], regNames[rs2]);
            else
                printf("Unknown Instruction\n");
            break;
        }
        case 0x1: { // I-type: [15:9] imm[6:0] | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            int16_t imm = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (imm & 0x40)
                imm |= 0xFF80;

            if (funct3 == 0x0)
                printf("addi %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x1)
                printf("slti %s, %d\n", regNames[rd_rs1], imm);
            else if (funct3 == 0x2) {
                uint16_t usimm = (inst >> 9) & 0x7F;
                printf("sltiu %s, %u\n", regNames[rd_rs1], usimm);
            }
            else if (funct3 == 0x3)
            {
                uint8_t shamt = imm & 0xF;
                uint8_t shift_type = (imm >> 4) & 0x7;
                if (shift_type == 0x1)
                    printf("slli %s, %d\n", regNames[rd_rs1], shamt);
                else if (shift_type == 0x2)
                    printf("srli %s, %d\n", regNames[rd_rs1], shamt);
                else if (shift_type == 0x4)
                    printf("srai %s, %d\n", regNames[rd_rs1], shamt);
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
                printf("Unknown Instruction\n");
            break;
        }
         case 0x2: { // B-type (branch): [15:12] offset[4:1] | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
            int16_t offset = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF8;;

            offset <<= 1;

            if (funct3 == 0x0)
                printf("beq %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x1)
                printf("bne %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
                else if (funct3 == 0x2)
                    printf("bz %s, %d\n", regNames[rs1], offset);

            else if( funct3==0x3)
                printf("bnz %s, %d\n", regNames[rs1], offset);

            else if (funct3 == 0x4)
                printf("blt %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x5)
                printf("bge %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x6)
                printf("bltu %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
            else if (funct3 == 0x7)
                printf("bgeu %s, %s, %d\n", regNames[rs1], regNames[rs2], offset);
            else
                printf("Unknown Instruction\n");

            break;
        }

       
        case 0x3: { // B type/store:  [15:12] offset | [11:9] rs2 | [8:6] rs1 | [5:3] funct3 | [2:0] opcode
            int16_t offset = ((inst >> 12) & 0xF) ;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF8;


            if (funct3 == 0x0)
                printf("sb %s, %d(%s)\n", regNames[rs2], offset, regNames[rs1]);
            else if (funct3 == 0x1)
                printf("sw %s, %d(%s)\n", regNames[rs2], offset, regNames[rs1]);
            else
                printf("Unknown Instruction\n");
            break;
        }

        case 0x4: { //L type:  [15:12] offset | [11:9] rs2 | [8:6] rd | [5:3] funct3 | [2:0] opcode
            int16_t offset = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (offset & 0x8)
                offset |= 0xFFF0;

            if (funct3 == 0x0)
                printf("lb %s, %d(%s)\n", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x1)
                printf("lw %s, %d(%s)\n", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x4)
                printf("lbu %s, %d(%s)\n", regNames[rd], offset, regNames[rs2]);
            else
                printf("Unknown Instruction\n");

            break;
        }
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
        case 0x7: { // U-Type
            uint8_t funct3  = (inst >> 3) & 0x7;
            uint8_t Sercice  = (inst >> 6) & 0x7;
            if(funct3 == 0x0) {
                printf("ecall %d\n", Sercice);
            }
            else
                printf("Unknown Instruction\n");
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
int executeInstruction(uint16_t inst) {
    uint8_t opcode = inst & 0x7;
    int pcUpdated = 0; // flag: if instruction updated PC directly
    switch(opcode) {
        case 0x0: {
            // R-type
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;
            if(funct4 == 0x0 && funct3 == 0x0) // add
                regs[rd_rs1] = regs[rd_rs1] + regs[rs2];
            else if(funct4 == 0x1 && funct3 == 0x0) // sub
                regs[rd_rs1] = regs[rd_rs1] - regs[rs2];
            else if(funct3 == 0x1)    // slt
                regs[rd_rs1] = ((int16_t)regs[rd_rs1] < (int16_t)regs[rs2]);
            else if(funct3 == 0x2)    // sltu
                regs[rd_rs1] = (regs[rd_rs1] < regs[rs2]);
            else if (funct3 == 0x3 && funct4 == 0x2)    // sll
                regs[rd_rs1] = regs[rd_rs1] << regs[rs2];
            else if (funct3 == 0x3 && funct4 == 0x4)    // srl
                regs[rd_rs1] = regs[rd_rs1] >> regs[rs2];
            else if (funct3 == 0x3 && funct4 == 0x8)    // sra
                regs[rd_rs1] = (int16_t)regs[rd_rs1] >> regs[rs2];
            else if (funct3 == 0x4)    // or
                regs[rd_rs1] = (regs[rd_rs1] | regs[rs2]);
            else if (funct3 == 0x5)    // and
                regs[rd_rs1] = (regs[rd_rs1] & regs[rs2]);
            else if (funct3 == 0x6)    // xor
                regs[rd_rs1] = (regs[rd_rs1] ^ regs[rs2]);
            else if (funct3 == 0x7)    // mv
                regs[rd_rs1] = regs[rs2];
            else if (funct3 == 0x0 && funct4 == 0x4) {    // jr

                infinityCheck[0]++;
                if(infinityCheck[0]>=100) {
                    printf("Infinite loop detected, are you sure you want to continue? Enter 1 if you want to continue, or 0 to stop the program\n");
                    char infcheck;
                    scanf(" %c", &infcheck);
                    while(infcheck != '0' && infcheck != '1') {
                        printf("Invalid option, press 0 or 1\n");
                        scanf(" %c", &infcheck);
                    }
                    if(infcheck == '1') {
                        infinityCheck[0] = 0;
                    }
                    else {
                        printf("Exiting program\n");
                        exit(1);
                    }
                }

                pc = regs[rs2];
                pcUpdated = 1;
            }
            else if (funct3 == 0x0 && funct4 == 0x8)    // jalr
            {
                infinityCheck[1]++;
                if(infinityCheck[1]>=100) {
                    printf("Infinite loop detected, are you sure you want to continue? Enter 1 if you want to continue, or 0 to stop the program\n");
                    char infcheck;
                    scanf(" %c", &infcheck);
                    while(infcheck != '0' && infcheck != '1') {
                        printf("Invalid option, press 0 or 1\n");
                        scanf(" %c", &infcheck);
                    }
                    if(infcheck == '1') {
                        infinityCheck[1] = 0;
                    }
                    else {
                        printf("Exiting program\n");
                        exit(1);
                    }
                }
                regs[rd_rs1] = pc + 2;
                pc = regs[rs2];
                pcUpdated = 1;
            }
            if (debug)
                printf("%s = %d\n", regNames[rd_rs1], regs[rd_rs1]);
            break;
        }
        case 0x1: { // I-type
            uint8_t imm7   = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7;

            if (funct3 == 0x0) //addi
            {
                regs[rd_rs1]+=simm;
            }
            else if(funct3 == 0x1) //slti
            {
                if((int16_t)regs[rd_rs1]<simm)
                    regs[rd_rs1]=1;
                else
                    regs[rd_rs1]=0;
            }

            else if(funct3 == 0x2) //sltui
            {
                if(regs[rd_rs1]<(uint16_t)simm)
                    regs[rd_rs1]=1;
                else
                    regs[rd_rs1]=0;
            }

            else if(funct3 == 0x3)
            {
                uint8_t shamt = imm7 & 0xF;
                uint8_t shift_type = (imm7 >> 4) & 0x7; // the remaining three bits of the immediate
                if (shift_type== 0x1) //slli
                    regs[rd_rs1]= regs[rd_rs1]<< (simm & 0x7);
                else if (shift_type== 0x2)//slri
                    regs[rd_rs1]= regs[rd_rs1]>> (simm & 0x7);
                else if (shift_type== 0x4) //sari
                    regs[rd_rs1]= (int16_t)regs[rd_rs1]>> (simm & 0x7);
            }

            else if(funct3 == 0x4) //ori
            {
                regs[rd_rs1] = regs[rd_rs1] | simm;
            }

            else if(funct3 == 0x5) //andi
            {
                regs[rd_rs1] = regs[rd_rs1] & simm;
            }
            else if(funct3 == 0x6) //xori
            {
                regs[rd_rs1] = regs[rd_rs1] ^ simm;

            }
            else if(funct3 == 0x7) //li
            {
                regs[rd_rs1] = simm;
            }if (debug)
                printf("%s = %d\n", regNames[rd_rs1], regs[rd_rs1]);
            break;
        }
        case 0x2: { // B-type branch
            uint8_t offset = (inst >> 12) & 0xF;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (offset & 0x8) ? (offset | 0xFFF0) : offset;
            simm <<= 1; // Scale offset by 2 for proper alignment

            switch (funct3) {
                case 0x0: // BEQ
                    if (regs[rs1] == regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x1: // BNE
                    if (regs[rs1] != regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x2: // BZ
                    if (regs[rs1] == 0) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x3: // BNZ
                    if (regs[rs1] != 0) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x4: // BLT
                    if ((int16_t)regs[rs1] < (int16_t)regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x5: // BGE
                    if ((int16_t)regs[rs1] >= (int16_t)regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x6: // BLTU
                    if (regs[rs1] < regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;

                case 0x7: // BGEU
                    if (regs[rs1] >= regs[rs2]) {
                        pc += simm;
                        pcUpdated = 1;
                    }
                break;
            }
            break;
        }

        case 0x3: {// B-type store
            uint8_t offset = (inst >> 12) & 0xF;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (offset & 0x8) ? (offset | 0xFFF0) : offset;

            switch (funct3)
            {
                case 0x0: //sb
                { memory[regs[rs1]+simm]= (uint8_t) (regs[rs2] & 0xFF);
                    break;}

                case 0x1: //sw, word=16*4 =32 bit
                { *(uint32_t*)&memory[regs[rs1]+simm]= regs[rs2];
                    break;

                    default:
                        break;}
            }


            break;
        }



        case 0x4: { // L-type (load)
            uint8_t offset = (inst >> 12) & 0xF;
            uint8_t rd = (inst >> 6) & 0x7;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (offset & 0x8) ? (offset | 0xFFF0) : offset;

            switch (funct3)
            {
                case 0x0: //lb
                {int8_t x= (int8_t)memory[regs[rs2]+simm];
                    regs[rd]= (int16_t)x;
                    break;}

                case 0x1: //lw
                {
                    uint16_t word = memory[regs[rs2]+simm+1];
                    word = word<<8;
                    word |= memory[regs[rs2]+simm];
                    regs[rd] = word;
                    break;
                }

                case 0x4: {//lbu

                    regs[rd]= (uint8_t) memory[regs[rs2]+simm];
                    break;}
                default:
                    break;
            }
            if(debug)
                printf("%s = %d\n", regNames[rd], regs[rd]);



            break;
        }
        case 0x5: { // J-type (jump)

            uint8_t f = (inst >> 15) & 0x1;
            uint8_t imm9to4 = (inst >> 9) & 0x3F;  // Upper immediate bits
            uint8_t rd = (inst >> 6) & 0x7;        // Destination register
            uint8_t imm3to1 = (inst >> 3) & 0x7;   // Lower immediate bits

            // Construct the immediate value
            int16_t simm = (imm9to4 << 4) | (imm3to1 << 1);

            // Sign-extend if bit 9 is set
            if (simm & 0x200)
                simm |= 0xFC00;


            if (f == 0) { // j

                infinityCheck[2]++;
                if(infinityCheck[2]>=100) {
                    printf("Infinite loop detected, are you sure you want to continue? Enter 1 if you want to continue, or 0 to stop the program\n");
                    char infcheck;
                    scanf(" %c", &infcheck);
                    while(infcheck != '0' && infcheck != '1') {
                        printf("Invalid option, press 0 or 1\n");
                        scanf(" %c", &infcheck);
                    }
                    if(infcheck == '1') {
                        infinityCheck[2] = 0;
                    }
                    else {
                        printf("Exiting program\n");
                        exit(1);
                    }
                }
                pc += simm;
            }
            else { // jal
                if (f == 1 && rd != 0) {

                    infinityCheck[3]++;
                    if(infinityCheck[3]>=100) {
                        printf("Infinite loop detected, are you sure you want to continue? Enter 1 if you want to continue, or 0 to stop the program\n");
                        char infcheck;
                        scanf(" %c", &infcheck);
                        while(infcheck != '0' && infcheck != '1') {
                            printf("Invalid option, press 0 or 1\n");
                            scanf(" %c", &infcheck);
                        }
                        if(infcheck == '1') {
                            infinityCheck[3] = 0;
                        }
                        else {
                            printf("Exiting program\n");
                            exit(1);
                        }
                    }

                    regs[rd] = pc + 2;
                    pc += simm;
                    if(debug)
                        printf("%s = %d\n", regNames[rd], regs[rd]);

                }

            }

        pcUpdated = 1;
        break;
    }


        case 0x6: { // U-type
            uint8_t f = (inst >> 15) & 0x1;
            uint8_t rd  = (inst >> 6) & 0x7;
            uint16_t I  = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8);
            if(f==0)    // lui
                regs[rd] = (I << 7);
            else        // auipc
                regs[rd] = pc + (I << 7);
            if(debug)
                printf("%s = %d\n", regNames[rd], regs[rd]);
            break;
        }
        case 0x7: { // System instruction (ecall)
            uint8_t funct3  = (inst >> 3) & 0x7;
            uint8_t Sercice  = (inst >> 6) & 0x7;
            if(funct3 == 0x0) {
                switch (Sercice) {
                    case 0x1: {
                        int16_t number = regs[6];
                        printf("%d\n", number);    // prints integer (in decimal)
                        break;
                    }
                    case 0x5: { // FIX
                        printf("%s\n", &memory[regs[6]]);    // gets pointer to null terminated string
                        break;
                    }
                    case 0x3: {
                        exit(1);    // exit program
                        break;
                    }
                }
            }
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
void loadMemoryFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        perror("Error opening binary file");
        exit(1);
    }
    size_t n = fread(memory, 1, MEM_SIZE, fp);
    fclose(fp);
    printf("Loaded %zu bytes into memory\n", n);
}

// -----------------------
// Main Simulation Loop
// -----------------------
int main(int argc, char **argv) {
    printf("If you only want to see the output without the instructions, press 1, otherwise; press 0\n");
    char option;
    scanf(" %c", &option);
    while(option != '0' && option != '1') {
        printf("Invalid option, press 0 or 1\n");
        scanf(" %c", &option);
    }
    if(option=='0') {
        printf("Do you want to debug? Press 1 if you want to see the register changes at each instruction, otherwise; press 0\n");
        char debug_option;
        scanf(" %c", &debug_option);
        while(debug_option != '0' && debug_option != '1') {
            printf("Invalid option, press 0 or 1\n");
            scanf(" %c", &debug_option);
        }
        if(debug_option=='1')
            debug=true;
    }
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <machine_code_file>\n", argv[0]);
        exit(1);
    }
    loadMemoryFromFile(argv[1]);
    memset(regs, 0, sizeof(regs)); // initialize registers to 0
    pc = 0;  // starting at address 0
    char disasmBuf[128];
    while(pc < MEM_SIZE) {
        // Fetch a 16-bit instruction from memory (little-endian)
        uint16_t inst = memory[pc] | (memory[pc+1] << 8);
        if(option=='0') {
            disassemble(inst, pc, disasmBuf, sizeof(disasmBuf));
            printf("0x%04X: %04X    %s\n", pc, inst, disasmBuf);
        }
        if(!executeInstruction(inst))
            break;
        // Terminate if PC goes out of bounds
        if(pc >= MEM_SIZE) break;
    }
    return 0;
}
