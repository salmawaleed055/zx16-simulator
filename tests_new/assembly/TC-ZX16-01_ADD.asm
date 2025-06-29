# TC-ZX16-01_ADD.asm
# Test Objective: Validate R-type ADD instruction
# x1 = x1 + x2 (5 + 3 = 8)
.text
li t0, 5
li t1, 3
add t0, t1
ecall 0x3FF







