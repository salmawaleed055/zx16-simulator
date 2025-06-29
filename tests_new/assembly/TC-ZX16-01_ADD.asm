# TC-ZX16-01_ADD.s
# Test Objective: Validate R-type ADD instruction
# x1 = x1 + x2 (5 + 3 = 8)
li x1, 5
li x2, 3
add x1, x2
ecall 0x3FF