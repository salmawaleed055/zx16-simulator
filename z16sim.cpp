// z16sim.cpp

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

// Initialize static member (only needs z16sim::)
const char* z16sim::regNames[z16sim::NUM_REGS] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};

// z16sim Constructor
z16sim::z16sim() {
    std::memset(this->memory, 0, z16sim::MEM_SIZE);
    std::memset(this->regs, 0, sizeof(this->regs));
    this->pc = 0;
    this->debug = false;
    initializeRegisterMap();
}
void z16sim::initializeRegisterMap() {
    // Map both standard names and x0-x7 aliases
    for (int i = 0; i < NUM_REGS; ++i) {
        regMap[regNames[i]] = i;
        regMap["x" + std::to_string(i)] = i;
    }
}

int z16sim::getRegisterIndex(const std::string& regName) {
    auto it = regMap.find(regName);
    if (it != regMap.end()) {
        return it->second;
    }
    return -1; // Invalid register
}

void z16sim::loadAssemblyFile(const char* filename) {
    std::cout << "Loading assembly file: " << filename << std::endl;

    // Reset state
    labels.clear();
    unresolved_labels.clear();
    std::memset(memory, 0, MEM_SIZE);
    pc = 0;

    parseAssemblyFile(filename);
    resolveLabels();

    std::cout << "Assembly loaded successfully." << std::endl;
}

void z16sim::parseAssemblyFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open assembly file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    uint16_t currentAddress = 0;

    while (std::getline(file, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        line = trim(line);
        if (line.empty()) continue;

        // Handle assembler directives
        if (line[0] == '.') {
            if (line.find(".org") == 0) {
                std::string addrStr = line.substr(4);
                addrStr = trim(addrStr);
                currentAddress = parseImmediate(addrStr);
                continue;
            } else if (line == ".text") {
                // Just continue, we're already in text mode
                continue;
            }
        }

        // Handle labels
        if (line.back() == ':') {
            std::string label = line.substr(0, line.length() - 1);
            label = trim(label);
            labels[label] = currentAddress;
            continue;
        }

        // Parse instruction
        uint16_t instruction = parseInstruction(line, currentAddress);

        // Store instruction in memory (little-endian)
        if (currentAddress + 1 < MEM_SIZE) {
            memory[currentAddress] = instruction & 0xFF;
            memory[currentAddress + 1] = (instruction >> 8) & 0xFF;
            currentAddress += 2;
        } else {
            std::cerr << "Error: Program too large for memory" << std::endl;
            exit(1);
        }
    }
}

uint16_t z16sim::parseInstruction(const std::string& line, uint16_t address) {
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) return 0;

    std::string op = tokens[0];
    std::vector<std::string> operands(tokens.begin() + 1, tokens.end());

    // Handle pseudo-instructions first
    if (op == "nop") {
        return 0x0000; // addi t0, t0, 0 equivalent
    }
    if (op == "ret") {
        return 0x8058; // jr ra equivalent (funct4=0xB, rd_rs1=1(ra), funct3=0, opcode=0)
    }
    if (op == "mv" && operands.size() == 2) {
        // Convert "mv rd, rs" to our R-type mv format
        return assembleRType("mv", operands);
    }

    // R-type instructions
    if (op == "add" || op == "sub" || op == "slt" || op == "sltu" ||
        op == "sll" || op == "srl" || op == "sra" || op == "or" ||
        op == "and" || op == "xor" || op == "mv" || op == "jr" || op == "jalr") {
        return assembleRType(op, operands);
    }

    // I-type instructions
    if (op == "addi" || op == "slti" || op == "sltui" || op == "slli" ||
        op == "srli" || op == "srai" || op == "ori" || op == "andi" ||
        op == "xori" || op == "li") {
        return assembleIType(op, operands);
    }

    // B-type instructions
    if (op == "beq" || op == "bne" || op == "blt" || op == "bge" ||
        op == "bltu" || op == "bgeu" || op == "bz" || op == "bnz") {
        return assembleBType(op, operands, address);
    }

    // S-type instructions
    if (op == "sb" || op == "sw") {
        return assembleSType(op, operands);
    }

    // L-type instructions
    if (op == "lb" || op == "lw" || op == "lbu") {
        return assembleLType(op, operands);
    }

    // J-type instructions
    if (op == "j" || op == "jal") {
        return assembleJType(op, operands, address);
    }

    // U-type instructions
    if (op == "lui" || op == "auipc") {
        return assembleUType(op, operands);
    }

    // SYS-type instructions
    if (op == "ecall") {
        return assembleSysType(op, operands);
    }

    std::cerr << "Unknown instruction: " << op << std::endl;
    return 0;
}

uint16_t z16sim::assembleRType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x0; // R-type opcode

    // Handle different operand patterns
    int rd, rs1, rs2;

    if (op == "jr" && operands.size() == 1) {
        rd = getRegisterIndex(operands[0]);
        rs1 = rd; rs2 = 0;
        inst |= (0xB << 12) | (0x0 << 3); // funct4=0xB, funct3=0x0
    } else if (op == "jalr" && operands.size() >= 1) {
        rd = getRegisterIndex(operands[0]);
        rs1 = rd; rs2 = 0;
        if (operands.size() > 1) rs2 = getRegisterIndex(operands[1]);
        inst |= (0xC << 12) | (0x0 << 3); // funct4=0xC, funct3=0x0
    } else if (op == "mv" && operands.size() == 2) {
        rd = getRegisterIndex(operands[0]);
        rs2 = getRegisterIndex(operands[1]);
        rs1 = rd;
        inst |= (0xA << 12) | (0x7 << 3); // funct4=0xA, funct3=0x7
    } else {
        // Standard 3-operand or convert 2-operand to 3-operand
        if (operands.size() == 2) {
            // Convert "add rd, rs" to "add rd, rd, rs"
            rd = getRegisterIndex(operands[0]);
            rs1 = rd;
            rs2 = getRegisterIndex(operands[1]);
        } else if (operands.size() == 3) {
            rd = getRegisterIndex(operands[0]);
            rs1 = getRegisterIndex(operands[1]);
            rs2 = getRegisterIndex(operands[2]);
        } else {
            std::cerr << "Invalid operand count for R-type: " << op << std::endl;
            return 0;
        }

        // Set funct4 and funct3 based on operation
        if (op == "add") inst |= (0x0 << 12) | (0x0 << 3);
        else if (op == "sub") inst |= (0x1 << 12) | (0x0 << 3);
        else if (op == "slt") inst |= (0x2 << 12) | (0x1 << 3);
        else if (op == "sltu") inst |= (0x3 << 12) | (0x2 << 3);
        else if (op == "sll") inst |= (0x4 << 12) | (0x3 << 3);
        else if (op == "srl") inst |= (0x5 << 12) | (0x3 << 3);
        else if (op == "sra") inst |= (0x6 << 12) | (0x3 << 3);
        else if (op == "or") inst |= (0x7 << 12) | (0x4 << 3);
        else if (op == "and") inst |= (0x8 << 12) | (0x5 << 3);
        else if (op == "xor") inst |= (0x9 << 12) | (0x6 << 3);
    }

    inst |= (rs2 << 9) | (rd << 6);
    return inst;
}

uint16_t z16sim::assembleIType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x1; // I-type opcode

    int rd, imm;

    if (operands.size() == 2) {
        // Convert "addi rd, imm" to "addi rd, rd, imm"
        rd = getRegisterIndex(operands[0]);
        imm = parseImmediate(operands[1]);
    } else if (operands.size() == 3) {
        rd = getRegisterIndex(operands[0]);
        // rs1 should equal rd for most I-type
        imm = parseImmediate(operands[2]);
    } else {
        std::cerr << "Invalid operand count for I-type: " << op << std::endl;
        return 0;
    }

    // Clamp immediate to 7-bit signed range
    if (imm > 63) imm = 63;
    if (imm < -64) imm = -64;

    // Set funct3 based on operation
    if (op == "addi" || op == "li") inst |= (0x0 << 3);
    else if (op == "slti") inst |= (0x1 << 3);
    else if (op == "sltui") inst |= (0x2 << 3);
    else if (op == "slli" || op == "srli" || op == "srai") {
        inst |= (0x3 << 3);
        // Handle shift immediates specially
        int shamt = imm & 0xF;
        if (op == "slli") imm = (0x1 << 4) | shamt;
        else if (op == "srli") imm = (0x2 << 4) | shamt;
        else if (op == "srai") imm = (0x4 << 4) | shamt;
    }
    else if (op == "ori") inst |= (0x4 << 3);
    else if (op == "andi") inst |= (0x5 << 3);
    else if (op == "xori") inst |= (0x6 << 3);

    inst |= ((imm & 0x7F) << 9) | (rd << 6);
    return inst;
}

uint16_t z16sim::assembleBType(const std::string& op, const std::vector<std::string>& operands, uint16_t address) {
    uint16_t inst = 0x2; // B-type opcode

    int rs1, rs2 = 0;
    std::string labelStr;

    if (op == "bz" || op == "bnz") {
        rs1 = getRegisterIndex(operands[0]);
        labelStr = operands[1];
    } else {
        rs1 = getRegisterIndex(operands[0]);
        rs2 = getRegisterIndex(operands[1]);
        labelStr = operands[2];
    }

    // Calculate offset or store for later resolution
    int16_t offset = 0;
    if (labelStr[0] == '0' && (labelStr[1] == 'x' || labelStr[1] == 'X')) {
        // Direct address
        uint16_t target = parseImmediate(labelStr);
        offset = target - address;
    } else {
        // Label - store for later resolution
        unresolved_labels.push_back(std::make_pair(address, labelStr));
        offset = 0; // Will be resolved later
    }

    // Clamp offset to 5-bit signed range (divided by 2)
    offset = offset / 2;
    if (offset > 15) offset = 15;
    if (offset < -16) offset = -16;

    // Set funct3 based on operation
    if (op == "beq") inst |= (0x0 << 3);
    else if (op == "bne") inst |= (0x1 << 3);
    else if (op == "blt") inst |= (0x2 << 3);
    else if (op == "bge") inst |= (0x3 << 3);
    else if (op == "bltu") inst |= (0x4 << 3);
    else if (op == "bgeu") inst |= (0x5 << 3);
    else if (op == "bz") inst |= (0x6 << 3);
    else if (op == "bnz") inst |= (0x7 << 3);

    inst |= ((offset & 0xF) << 12) | (rs2 << 9) | (rs1 << 6);
    return inst;
}

uint16_t z16sim::assembleSType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x3; // S-type opcode

    // Parse "sw rs2, offset(rs1)" format
    int rs2 = getRegisterIndex(operands[0]);

    std::string offsetReg = operands[1];
    size_t parenPos = offsetReg.find('(');
    int offset = 0;
    int rs1;

    if (parenPos != std::string::npos) {
        std::string offsetStr = offsetReg.substr(0, parenPos);
        std::string regStr = offsetReg.substr(parenPos + 1);
        regStr.pop_back(); // Remove ')'

        if (!offsetStr.empty()) {
            offset = parseImmediate(offsetStr);
        }
        rs1 = getRegisterIndex(regStr);
    } else {
        rs1 = getRegisterIndex(offsetReg);
    }

    // Clamp offset to 4-bit signed range
    if (offset > 7) offset = 7;
    if (offset < -8) offset = -8;

    // Set funct3 based on operation
    if (op == "sb") inst |= (0x0 << 3);
    else if (op == "sw") inst |= (0x2 << 3);

    inst |= ((offset & 0xF) << 12) | (rs2 << 9) | (rs1 << 6);
    return inst;
}

uint16_t z16sim::assembleLType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x4; // L-type opcode

    // Parse "lw rd, offset(rs2)" format
    int rd = getRegisterIndex(operands[0]);

    std::string offsetReg = operands[1];
    size_t parenPos = offsetReg.find('(');
    int offset = 0;
    int rs2;

    if (parenPos != std::string::npos) {
        std::string offsetStr = offsetReg.substr(0, parenPos);
        std::string regStr = offsetReg.substr(parenPos + 1);
        regStr.pop_back(); // Remove ')'

        if (!offsetStr.empty()) {
            offset = parseImmediate(offsetStr);
        }
        rs2 = getRegisterIndex(regStr);
    } else {
        rs2 = getRegisterIndex(offsetReg);
    }

    // Clamp offset to 4-bit signed range
    if (offset > 7) offset = 7;
    if (offset < -8) offset = -8;

    // Set funct3 based on operation
    if (op == "lb") inst |= (0x0 << 3);
    else if (op == "lw") inst |= (0x2 << 3);
    else if (op == "lbu") inst |= (0x3 << 3);

    inst |= ((offset & 0xF) << 12) | (rs2 << 9) | (rd << 6);
    return inst;
}

uint16_t z16sim::assembleJType(const std::string& op, const std::vector<std::string>& operands, uint16_t address) {
    uint16_t inst = 0x5; // J-type opcode

    std::string target;
    int rd = 0; // Default for j instruction

    if (op == "j") {
        target = operands[0];
        inst |= (0x0 << 15); // f = 0
    } else if (op == "jal") {
        if (operands.size() == 1) {
            target = operands[0];
            rd = 1; // Default to ra
        } else {
            rd = getRegisterIndex(operands[0]);
            target = operands[1];
        }
        inst |= (0x1 << 15); // f = 1
    }

    // Calculate offset or store for later resolution
    int16_t offset = 0;
    if (target[0] == '0' && (target[1] == 'x' || target[1] == 'X')) {
        // Direct address
        uint16_t targetAddr = parseImmediate(target);
        offset = targetAddr - address;
    } else {
        // Label - store for later resolution
        unresolved_labels.push_back(std::make_pair(address, target));
        offset = 0; // Will be resolved later
    }

    // Clamp offset to 10-bit signed range (bits [9:1])
    if (offset > 511) offset = 511;
    if (offset < -512) offset = -512;

    uint16_t imm9to4 = (offset >> 4) & 0x3F;
    uint16_t imm3to1 = (offset >> 1) & 0x7;

    inst |= (imm9to4 << 9) | (rd << 6) | (imm3to1 << 3);
    return inst;
}

uint16_t z16sim::assembleUType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x6; // U-type opcode

    int rd = getRegisterIndex(operands[0]);
    int imm = parseImmediate(operands[1]);

    // Clamp immediate to 8-bit unsigned range
    if (imm > 255) imm = 255;
    if (imm < 0) imm = 0;

    if (op == "lui") inst |= (0x0 << 15);
    else if (op == "auipc") inst |= (0x1 << 15);

    // Encode 8-bit immediate into bits [15:8] and [8:6]
    uint16_t imm_low = imm & 0x7;        // bits [2:0]
    uint16_t imm_high = (imm >> 3) & 0x1F; // bits [7:3]

    inst |= (imm_high << 9) | (rd << 6) | (imm_low << 3);
    return inst;
}

uint16_t z16sim::assembleSysType(const std::string& op, const std::vector<std::string>& operands) {
    uint16_t inst = 0x7; // SYS-type opcode

    int svc = 0;
    if (!operands.empty()) {
        svc = parseImmediate(operands[0]);
    }

    // Clamp service code to 10-bit range
    svc &= 0x3FF;

    inst |= (svc << 6) | (0x0 << 3); // funct3 = 0 for ecall
    return inst;
}

std::vector<std::string> z16sim::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token) {
        // Remove commas
        if (token.back() == ',') {
            token.pop_back();
        }
        tokens.push_back(token);
    }

    return tokens;
}

std::string z16sim::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int32_t z16sim::parseImmediate(const std::string& str) {
    if (str.empty()) return 0;

    if (str[0] == '0' && str.length() > 1 && (str[1] == 'x' || str[1] == 'X')) {
        return std::stoi(str, nullptr, 16);
    } else {
        return std::stoi(str);
    }
}

void z16sim::resolveLabels() {
    for (auto& unresolved : unresolved_labels) {
        uint16_t instrAddr = unresolved.first;
        std::string label = unresolved.second;

        auto it = labels.find(label);
        if (it == labels.end()) {
            std::cerr << "Error: Undefined label '" << label << "' at address 0x"
                      << std::hex << instrAddr << std::endl;
            continue;
        }

        uint16_t targetAddr = it->second;
        int16_t offset = targetAddr - instrAddr;

        // Read current instruction
        uint16_t inst = (memory[instrAddr + 1] << 8) | memory[instrAddr];
        uint8_t opcode = inst & 0x7;

        if (opcode == 0x2) { // B-type
            offset = offset / 2; // Branch offsets are in instruction units
            if (offset > 15) offset = 15;
            if (offset < -16) offset = -16;

            inst = (inst & 0x0FFF) | ((offset & 0xF) << 12);
        } else if (opcode == 0x5) { // J-type
            if (offset > 511) offset = 511;
            if (offset < -512) offset = -512;

            uint16_t imm9to4 = (offset >> 4) & 0x3F;
            uint16_t imm3to1 = (offset >> 1) & 0x7;

            inst = (inst & 0x80C7) | (imm9to4 << 9) | (imm3to1 << 3);
        }

        // Write back instruction
        memory[instrAddr] = inst & 0xFF;
        memory[instrAddr + 1] = (inst >> 8) & 0xFF;
    }
}

// dumpRegisters method definition
void z16sim::dumpRegisters() const {
    for (int i = 0; i < z16sim::NUM_REGS; ++i) { // Use z16sim::NUM_REGS
        std::cout << regNames[i] << ": 0x"
                  << std::hex << std::setw(4) << std::setfill('0')
                  << regs[i] << std::endl; // Access member 'regs'
    }
}

// loadMemoryFromFile method definition
void z16sim::loadMemoryFromFile(const char* filename) { // Added z16sim::
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1); // Exit if file cannot be opened
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > z16sim::MEM_SIZE) { // Use z16sim::MEM_SIZE
        std::cerr << "Warning: File size (" << size << " bytes) exceeds memory size (" << z16sim::MEM_SIZE << " bytes). Only loading " << z16sim::MEM_SIZE << " bytes." << std::endl;
        size = z16sim::MEM_SIZE;
    }

    file.read(reinterpret_cast<char*>(this->memory), size); // Access member 'memory'
    if (!file && file.gcount() != size) { // Check for read errors or incomplete read
        std::cerr << "Error: Failed to read " << size << " bytes from " << filename << " completely. Read " << file.gcount() << " bytes." << std::endl;
        exit(1);
    }
    std::cout << "Loaded " << file.gcount() << " bytes from " << filename << " into memory." << std::endl;
    this->pc = 0; // Initialize PC to 0 after loading the program
}

// cycle method definition
bool z16sim::cycle() { // Added z16sim::
    // Check for PC out of bounds before fetching instruction
    if (this->pc >= z16sim::MEM_SIZE - 1) { // -1 because 16-bit instructions need 2 bytes
        std::cerr << "Error: Program Counter out of bounds (0x" << std::hex << this->pc << ") at end of memory." << std::endl;
        return false; // Stop simulation
    }

    // Fetch 16-bit instruction (assuming little-endian for simplicity; adjust if needed)
    uint16_t instruction = (this->memory[this->pc + 1] << 8) | this->memory[this->pc];

    char disasm_buf[256];
    // Pass the *current* PC value to disassemble for address calculation
    z16sim::disassemble(instruction, this->pc, disasm_buf, sizeof(disasm_buf)); // Call member method

    std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << this->pc
              << " | Inst: 0x" << std::setw(4) << std::setfill('0') << instruction
              << " | " << disasm_buf << std::endl;

    // Execute the instruction and check its return status
    int status = z16sim::executeInstruction(instruction); // Call member method

    if (status == 0) { // Assuming 0 means continue (instruction was executed, PC might have updated)
        // If executeInstruction handles PC update for jumps/branches,
        // then only advance PC by 2 for sequential instructions (non-jumps/branches).
        // For simplicity, here we always advance by 2, assuming executeInstruction
        // will overwrite this->pc if it's a branch/jump.
        // A more robust way is to have executeInstruction return a flag
        // indicating if PC was modified.
        // For now, let's assume executeInstruction directly modifies PC if needed.
        return true; // Continue simulation
    } else {
        // A non-zero status indicates termination (e.g., ECALL for halt)
        return false; // Stop simulation
    }
}

// executeInstruction method definition
int z16sim::executeInstruction(uint16_t inst) { // Added z16sim::
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
            } else if (funct4 == 0x1 && funct3 == 0x0) { // sub
                this->regs[rs1_rd] = val1 - val2;
                this->pc += 2;
            } else if (funct4 == 0x2 && funct3 == 0x1) { // slt (set less than signed)
                this->regs[rs1_rd] = ((int16_t)val1 < (int16_t)val2) ? 1 : 0;
                this->pc += 2;
            } else if (funct4 == 0x3 && funct3 == 0x2) { // sltu (set less than unsigned)
                this->regs[rs1_rd] = (val1 < val2) ? 1 : 0;
                this->pc += 2;
            } else if (funct4 == 0x4 && funct3 == 0x3) { // sll (shift left logical)
                this->regs[rs1_rd] = val1 << (val2 & 0xF); // Only use lower 4 bits for shift amount
                this->pc += 2;
            } else if (funct4 == 0x5 && funct3 == 0x3) { // srl (shift right logical)
                this->regs[rs1_rd] = val1 >> (val2 & 0xF);
                this->pc += 2;
            } else if (funct4 == 0x6 && funct3 == 0x3) { // sra (shift right arithmetic)
                this->regs[rs1_rd] = (int16_t)val1 >> (val2 & 0xF);
                this->pc += 2;
            } else if (funct4 == 0x7 && funct3 == 0x4) { // or
                this->regs[rs1_rd] = val1 | val2;
                this->pc += 2;
            } else if (funct4 == 0x8 && funct3 == 0x5) { // and
                this->regs[rs1_rd] = val1 & val2;
                this->pc += 2;
            } else if (funct4 == 0x9 && funct3 == 0x6) { // xor
                this->regs[rs1_rd] = val1 ^ val2;
                this->pc += 2;
            } else if (funct4 == 0xA && funct3 == 0x7) { // mv (move) - alias for addi with 0? or specific R-type?
                // Assuming 'mv rd, rs2' is 'add rd, r0, rs2' or similar.
                // If it's a specific R-type, its format might differ from 'add'.
                // Based on assembler: mv rd, rs means mv (rd <- rs)
                this->regs[rs1_rd] = this->regs[rs2]; // rs1_rd is RD, rs2 is RS
                this->pc += 2;
            } else if (funct4 == 0xB && funct3 == 0x0) { // jr (jump register)
                this->pc = this->regs[rs1_rd]; // Jump to address in rs1_rd
            } else if (funct4 == 0xC && funct3 == 0x0) { // jalr (jump and link register)
                this->regs[z16sim::RA_REG] = this->pc + 2; // Store return address
                this->pc = this->regs[rs1_rd]; // Jump to address in rs1_rd
            } else {
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

            uint16_t target_addr = this->pc + offset;

            bool branch_taken = false;
            if (funct3 == 0x0) { // beq
                if (this->regs[rs1_rd] == this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x1) { // bne
                if (this->regs[rs1_rd] != this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x2) { // blt (signed)
                if ((int16_t)this->regs[rs1_rd] < (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x3) { // bge (signed)
                if ((int16_t)this->regs[rs1_rd] >= (int16_t)this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x4) { // bltu (unsigned)
                if (this->regs[rs1_rd] < this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x5) { // bgeu (unsigned)
                if (this->regs[rs1_rd] >= this->regs[rs2]) branch_taken = true;
            } else if (funct3 == 0x6) { // bz (branch if zero) - rs2 is ignored
                if (this->regs[rs1_rd] == 0) branch_taken = true;
            } else if (funct3 == 0x7) { // bnz (branch if not zero) - rs2 is ignored
                if (this->regs[rs1_rd] != 0) branch_taken = true;
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
            } else if (funct3 == 0x2) { // sw (store word)
                // Assuming little-endian storage for word
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
            } else if (funct3 == 0x2) { // lw (load word)
                // Assuming little-endian load for word
                 if (mem_addr + 1 >= z16sim::MEM_SIZE) {
                     std::cerr << "Memory access out of bounds for word load at 0x" << std::hex << mem_addr << " at PC: 0x" << this->pc << std::endl;
                     return 4;
                }
                this->regs[dest_reg] = (this->memory[mem_addr + 1] << 8) | this->memory[mem_addr];
            } else if (funct3 == 0x3) { // lbu (load byte unsigned)
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

            imm = (imm9to4 << 4) | (imm3to1 << 1); // Reconstruct 9-bit immediate (multiplied by 2)
            if (imm & 0x200) { // Check if bit 9 (sign bit) is set
                 imm |= 0xFC00; // Sign extend to 16-bit
            }

            uint16_t target_addr = this->pc + imm;

            if (f == 0) { // j (jump)
                if (!updatePC(target_addr, "Jump")) return 3;
            } else if (f == 1) { // jal (jump and link)
                this->regs[z16sim::RA_REG] = this->pc + 2; // Store return address
                if (!updatePC(target_addr, "JumpAndLink")) return 3;
            } else {
                std::cerr << "Unknown J-type instruction: 0x" << std::hex << inst << " at PC: 0x" << this->pc << std::endl;
                return 2;
            }
            break;
        }

        case 0x6: { // U-Type (lui, auipc)
            uint8_t f = (inst >> 15) & 0x1;
            uint16_t U_imm = ((inst >> 3) & 0x7) | ((inst >> 6) & 0x1F8); // Reconstruct 8-bit immediate I[7:0]
            // Note: U_imm is 8-bit unsigned. The assembler uses I[7:0] for LUI/AUIPC,
            // which becomes bits [15:8] of the 16-bit result.

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
            uint8_t func3 = (inst >> 3) & 0x7; // Should be 000 for ecall

            if (func3 == 0x0) { // ecall
                std::cout << "ECALL (Service: 0x" << std::hex << svc << ") encountered. Terminating simulation." << std::endl;
                // You can add more detailed syscall handling here based on 'svc' value
                // For now, let's assume ecall always terminates the simulation.
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
    return 0; // Return 0 to indicate successful execution (continue simulation)
}

// reset method definition
void z16sim::reset() { // Added z16sim::
    std::memset(this->memory, 0, z16sim::MEM_SIZE);
    std::memset(this->regs, 0, sizeof(this->regs));
    this->pc = 0;
    this->debug = false;
    std::cout << "Simulator reset." << std::endl;
}

// updatePC method definition
bool z16sim::updatePC(uint16_t new_pc, const char* instruction_name) { // Added z16sim::
    if (new_pc >= z16sim::MEM_SIZE) {
        std::cerr << "Error: " << instruction_name << " tried to set PC out of bounds to 0x"
                  << std::hex << new_pc << std::endl;
        return false; // Indicates an error or invalid jump
    }
    this->pc = new_pc;
    return true; // Indicates PC updated successfully
}

// disassemble method definition (as provided, ensure pc parameter is not confused with member this->pc)
void z16sim::disassemble(uint16_t inst, uint16_t current_pc, char *buf, size_t bufSize) {
    uint8_t opcode = inst & 0x7;
    // ... (rest of your disassemble function, ensure you use current_pc for branch target calculation)
    // For example, in B-type:
    // uint16_t target_addr = current_pc + offset;
    // And remove any global 'pc' variable references from here if it existed.
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
            int16_t imm_val = (inst >> 9) & 0x7F; // Using imm_val to distinguish from global imm
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

            uint16_t target_addr = current_pc + offset; // Use current_pc here

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
                snprintf(buf, bufSize, "bz %s, 0x%04X", regNames[rs1], target_addr);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "bnz %s, 0x%04X", regNames[rs1], target_addr);
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
            else if (funct3 == 0x2)
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
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "lw %s, %d(%s)", regNames[rd], offset, regNames[rs2]);
            else if (funct3 == 0x3)
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

            int16_t imm_val = (imm9to4 << 4) | (imm3to1 << 1);
            if (imm_val & 0x200) imm_val |= 0xFC00;

            uint16_t target_addr = current_pc + imm_val;

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


// --- Main function (remains outside the class) ---
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