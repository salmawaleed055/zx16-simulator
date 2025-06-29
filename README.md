# zx16-simulator
 **ZX16 Instruction Set Simulator** that emulates a custom **RISC-based CPU architecture**, supporting:

* Instruction decoding and execution
* System calls (`ecall`) for I/O and control
* Memory load/store operations
* A 2D tile-based graphics system for visual output (e.g., games, graphics tasks)


#Features

* **8 General-Purpose Registers** (`x0` to `x7`) with readable aliases (`t0`, `ra`, `sp`, `s0`, `s1`, `t1`, `a0`, `a1`)
* **64KB of addressable memory**
* **Instruction Set Support** including:

  * `li`, `add`, `sub`, `and`, `or`, `xor`, `lw`, `sw`, `beq`, `bne`, etc.
* **ECALL Support** for:

  * Halting the simulator
  * Printing register values
  * Interfacing with the graphics system
* **Instruction Disassembly** for debugging
* **Cycle-by-cycle Execution Mode** with optional register tracing
* **Graphics API** based on a tile grid (supports drawing pixels or tiles on a 2D screen)

---

## Build Instructions

### Prerequisites

* C++20-compatible compiler (e.g., `g++`, `clang++`)
* [CMake](https://cmake.org/) 3.10 or higher
* A supported development environment (e.g., CLion, Visual Studio Code)

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Or simply press **Build** in CLion if you're using an IDE.

---

## Usage

```bash
./zx16_simulator <path_to_binary_file>
```

The program will prompt:

1. Whether to suppress instruction debug prints
2. Whether to enable detailed register change logging after each instruction

Sample output:

```
If you only want to see the output without the instructions, press 1, otherwise press 0
0
Do you want to debug? Press 1 if you want to see the register changes at each instruction, otherwise press 0
1
Loaded 8 bytes into memory
0x0000: 0A79    li x1, 5
LI executed: x1 = 5
Registers: t0=0x0000 ra=0x0005 ...
```

---

## Architecture Overview

* **Main Components:**

  * `z16sim.cpp / z16sim.h`: Simulator core
  * `main.cpp`: Driver and user interaction
  * `memory[]`: 64KB simulated memory
  * `regs[8]`: Register file
  * `pc`: Program Counter
* **Instruction Execution Loop:**

  1. Fetch 16-bit instruction from `memory[pc]`
  2. Decode and disassemble
  3. Execute and update `regs[]` or `pc`
  4. Repeat until an `ecall 0x3FF` or invalid opcode halts the simulator

---

## Binary Input Format

* Simulator accepts `.bin` machine code files (e.g., from an assembler)
* Instructions are 16-bit and stored little-endian in memory

---
Future Enhancements

* [ ] Interrupt handling support
* [ ] IO-mapped peripherals (keyboard, screen, etc.)
* [ ] Compiler front-end or assembler
* [ ] Better debugging GUI

