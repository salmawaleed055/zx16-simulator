# TC-ZX16-05_SLTI.s
# Test Objective: Validate I-type SLTI (Set Less Than Immediate Signed) instruction
# x1 = (-2 < -1) ? 1 : 0 = 1
li x1, -2 # x1 = 0xFFFE
slti x1, -1 # x1 = (int16_t)0xFFFE < (int16_t)0xFFFF ? 1 : 0 => 1
ecall 0x3FF