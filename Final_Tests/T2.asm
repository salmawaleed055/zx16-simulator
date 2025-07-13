# ZX16 I-Type Instructions Test
# Tests all immediate arithmetic and logical operations

.text
.org 0x0000

main:

    # Test ADDI with positive immediate
    li x1, 10
    addi x1, 25         # x1 = 10 + 25 = 35

    # Test ADDI with negative immediate
    li x2, 50
    addi x2, -15        # x2 = 50 - 15 = 35

    # Test SLTI (signed immediate comparison)
    li x3, 5
    slti x3, 10         # x3 = (5 < 10) = 1

    li x4, 15
    slti x4, 10         # x4 = (15 < 10) = 0

    # Test SLTUI (unsigned immediate comparison)
    li x5, 0xFF         # -1 as signed, 255 as unsigned
    sltui x5, 10        # x5 = (255 < 10) = 0 (unsigned)

    # Test shift immediate operations
    li x6, 1
    slli x6, 4          # x6 = 1 << 4 = 16

    li x7, 0x80
    srli x7, 3          # x7 = 128 >> 3 = 16 (logical)

    li x0, 0x8000       # -32768
    srai x0, 2          # x0 = -32768 >> 2 = -8192 (arithmetic)

    # Test logical immediate operations
    li x1, 0x00
    ori x1, 0x0F        # x1 = 0x00 | 0x0F = 0x0F

    li x2, 0xFF
    andi x2, 0x0F       # x2 = 0xFF & 0x0F = 0x0F

    li x3, 0xAA
    xori x3, 0x5      # x3 = 175

    # Test LI instruction with various values
    li x4, 0            # Load zero
    li x5, 63           # Load maximum positive 7-bit value
    li x6, -64          # Load minimum negative 7-bit value
    li x7, -1           # Load -1

    # Exit
    clr a0
    ecall 3
