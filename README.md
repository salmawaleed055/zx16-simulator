# ZX16 Instruction Set Simulator

A custom RISC-based CPU emulator supporting instruction execution, system calls, memory operations, and basic audio functionality.

## Table of Contents
- [Features](#features)
- [Build Instructions](#build-instructions)
- [Usage Guidelines](#usage-guidelines)
- [Design Overview](#design-overview)
- [Project Challenges & Limitations](#project-challenges--limitations)
- [Testing Guidelines](#testing-guidelines)

## Features
# ZX16 Instruction Set Simulator

A custom RISC-based CPU emulator supporting instruction execution, system calls, memory operations, and basic audio functionality.

## Table of Contents
- [Features](#features)
- [Build Instructions](#build-instructions)
- [Usage Guidelines](#usage-guidelines)
- [Design Overview](#design-overview)
- [Project Challenges & Limitations](#project-challenges--limitations)
- [Testing Guidelines](#testing-guidelines)

## Features
- 8 General-Purpose Registers (x0-x7)
  - Aliases: t0, ra, sp, s0, s1, t1, a0, a1
- 64KB Addressable Memory
- Instruction Set:
  - Arithmetic: `add`, `sub`, `inc`, `dec`
  - Logical: `and`, `or`, `xor`, `not`
  - Memory: `lw`, `sw`, `lb`, `sb`
  - Control Flow: `beq`, `bne`, `j`, `jal`, `jr`
  - System: `ecall`
- System Calls (ecall):
  - I/O operations
  - Audio tone generation
  - Register/memory inspection
- Instruction disassembly for debugging
- Interactive step-by-step execution mode

## Build Instructions
### Prerequisites
- C++20 compatible compiler (g++ or clang++)
- CMake 3.10+

### Build Steps
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage Guidelines
### Running the Simulator
```bash
./zx16sim <binary_file>       # Normal execution
./zx16sim -i <binary_file>    # Interactive step-by-step mode
```

### Example Test Cases
#### Arithmetic Operations Test
```assembly
li x1, 10
li x2, 5
add x1, x2          # x1 = 10 + 5 = 15
```
Expected: x1 = 15 (0x000F)

#### Branch Test
```assembly
li x1, 5
li x2, 5
beq x1, x2, success # Branch taken
clr x6              # Fail: x6 = 0
ecall 10

success:
    li x6, 1        # Pass: x6 = 1
    ecall 10
```
Expected: x6 = 1 (branch taken)

#### Memory Operations Test
```assembly
li x1, 12
li x2, 5
add x1, x2
sw x1, 0(x2)        # Store x1 to memory[x2]
lw x3, 0(x2)        # Load back to x3
```
Expected: x3 = 17 (0x0011)

#### Audio Test (Twinkle Twinkle)
```assembly
li x6, 200
ecall 5             # Set volume

# Play notes (C C G G A A G)
li x6, 261         # C
li x7, 400
ecall 4             # Play tone
```
Expected: Plays the beginning of "Twinkle Twinkle Little Star"

## Design Overview
### Main Components
- `z16sim.cpp`/`z16sim.h`: Core simulator logic
- `memory[]`: 64KB simulated RAM
- `regs[]`: Register file
- `pc`: Program counter

### Execution Cycle
1. Fetch 16-bit instruction from `memory[pc]`
2. Decode and disassemble
3. Execute, updating registers or memory
4. Repeat until halt (ecall 10)

## Project Challenges & Limitations
### Challenges
- Compact 16-bit instruction encoding
- Accurate cycle-by-cycle simulation
- System call implementation

### Limitations
- No floating-point support
- Basic audio requires Linux `play` command
- Limited to 64KB memory

## Testing Guidelines
All test cases are organized by instruction type:

### R-Type Instructions Test
Tests arithmetic, logical, and register operations:
```assembly
# Test ADD
li x1, 10
li x2, 5
add x1, x2          # x1 = 10 + 5 = 15

# Test SUB
li x3, 20
li x4, 8
sub x3, x4          # x3 = 20 - 8 = 12
```
[Full test case](tests/rtype_test.s)

### I-Type Instructions Test
Tests immediate operations:
```assembly
# Test ADDI
li x1, 10
addi x1, 25         # x1 = 10 + 25 = 35

# Test SLTI
li x3, 5
slti x3, 10         # x3 = (5 < 10) = 1
```
[Full test case](tests/itype_test.s)

### Branch Instructions Test
Tests conditional branches:
```assembly
# Test BEQ
li x1, 5
li x2, 5
beq x1, x2, taken   # Branch taken

# Test BNE
li x3, 10
li x4, 15
bne x3, x4, taken   # Branch taken
```
[Full test case](tests/branch_test.s)

### System Call Tests
Tests ECALL functionality:
```assembly
# Print string
la x6, hello
ecall 3             # SYS_PUTSTR

# Play tone
li x6, 440         # 440Hz
li x7, 1000        # 1 second
ecall 4             # SYS_TONE
```
[Full test case](tests/ecall_test.s)


For questions or contributions, please open an issue in the repository.
```

This README.md:
1. Integrates all your test cases in appropriate sections
2. Maintains clear organization with headers and code blocks
3. Shows both the architecture overview and specific test examples
4. Includes build/usage instructions
5. Highlights limitations and features

The markdown formatting ensures proper rendering on GitHub/GitLab while keeping the test cases clearly visible and properly formatted.