# ZX16 R-Type Instructions Test
# Tests all R-type arithmetic, logical, and register operations

.text
.org 0x0020
main:

    # Test ADD instruction
    li x1, 10
    li x2, 5
    add x1, x2          # x1 = 10 + 5 = 15


    # Test SUB instruction
    li x3, 20
    li x4, 8
    sub x3, x4          # x3 = 20 - 8 = 12


    # Test SLT instruction (signed comparison)
    li x5, -5
    li x6, 3
    slt x5, x6          # x5 = (-5 < 3) = 1

    # Test SLTU instruction (unsigned comparison)
    li x0, 0xFF         # -1 as signed, 255 as unsigned
    nop
    li x1, 1
    sltu x0, x1         # x0 = (255 < 1) = 0 (unsigned)

    # Test shift operations
    li x2, 0x0F         # 15 in binary: 00001111
    li x3, 2
    sll x2, x3          # x2 = 15 << 2 = 60 (00111100)

    li x4, 0x3C         # 60 in binary: 00111100
    li x5, 2
    srl x4, x5          # x4 = 60 >> 2 = 15 (logical shift)

    li16 x6, 0x8000       # 16768
    li x7, 1
    sra x6, x7          # x6 = 16768 >> 1 = 8384 (arithmetic shift)

    # Test logical operations
    li x0, 0x0F
    li x1, 0x33
    or x0, x1           # x0 = 0x0F | 0x33 = 0x3F

    li x2, 0xFF
    nop
    li x3, 0x0F
    and x2, x3          # x2 = 0xFF & 0x0F = 0x0F

    li x4, 17
    li x5, 8
    xor x4, x5          # x4 = 25

    # Test MV instruction
    li x6, 0x1234
    nop
    mv x7, x6           # x7 = x6 = 0x1234

    # Test jump instructions
    la x0, test_function
    jalr x1, x0         # Call function, return address in x1


    # Exit
    clr a0
    ecall 10

test_function:
    # Simple function that increments x7
    inc x7
    jr x1               # Return using address in x1
