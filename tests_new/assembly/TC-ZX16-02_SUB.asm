# TC-ZX16-02_SUB.s
# Test Objective: Validate R-type SUB instruction
# x1 = x1 - x2 (7 - 2 = 5)
li t0, 7
li t1, 2
sub t0, t1
ecall 0x3FF

