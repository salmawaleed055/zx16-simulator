# TC-ZX16-07_LI.s
# Test Objective: Validate I-type LI (Load Immediate) instruction
# x1 = -5 (0xFFFB)
li x1, -5 # x1 = -5 (signed 0xFFFB)
ecall 0x3FF