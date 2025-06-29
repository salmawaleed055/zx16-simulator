# TC-ZX16-06_SLLI.s
# Test Objective: Validate I-type SLLI (Shift Left Logical Immediate) instruction
# x1 = x1 << 4 (1 << 4 = 16)
li x1, 1
slli x1, 4
ecall 0x3FF