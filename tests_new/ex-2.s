# Test Case: Simple Z16 Instructions
# File: simple_test.s
# Tests: Basic R-type, I-type, and control flow

.text
.org 0x000
j main

.org 0x0020
main:
    # Basic R-type arithmetic
    add x1, x2          # x1 = x1 + x2
    sub x2, x3          # x2 = x2 - x3
    and x3, x4          # x3 = x3 & x4
    or x4, x5           # x4 = x4 | x5

    # Simple I-type instructions
    addi x1, 10         # x1 = x1 + 10
    addi x2, -5         # x2 = x2 - 5
    li x5, 25           # x5 = 25
    li x6, -10          # x6 = -10

    # Basic memory operations
    sw x1, 0(x2)        # Store x1 to memory[x2 + 0]
    lw x7, 0(x2)        # Load from memory[x2 + 0] to x7

    # Simple branch test
    beq x1, x7, equal   # Branch if x1 == x7
    addi x3, 1          # This should be skipped if branch taken

equal:
    # Simple function call
    jal x1, func        # Call function, return address in x1

    # Exit
    ecall 0x00A         # Exit system call

func:
    addi x6, 5          # Simple operation in function
    jr x1               # Return using jump register
