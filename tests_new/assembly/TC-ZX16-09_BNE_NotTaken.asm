# TC-ZX16-09_BNE_NotTaken.s
# Test Objective: Validate BNE (Branch Not Equal) instruction - branch not taken
# x1=10, x2=10, branch to PC+2 (fall through)
li x1, 10
li x2, 10
bne x1, x2, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
li x1, 1       # Executed, PC becomes 0x0006
nop            # This instruction is now at PC 0x0008
ecall 0x3FF