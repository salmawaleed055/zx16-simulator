# TC-ZX16-02_SUB.s
# Test Objective: Validate R-type SUB instruction
# x1 = x1 - x2 (7 - 2 = 5)
li x1, 7
li x2, 2
sub x1, x2
ecall 0x3FF