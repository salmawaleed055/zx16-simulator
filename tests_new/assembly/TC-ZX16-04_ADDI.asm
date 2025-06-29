# TC-ZX16-04_ADDI.s
# Test Objective: Validate I-type ADDI instruction
# x1 = x1 + 7 (10 + 7 = 17)
li x1, 10
addi x1, 7
ecall 0x3FF