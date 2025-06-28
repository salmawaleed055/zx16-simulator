# TC-ZX16-08_BNE_Taken.s
# Test Objective: Validate BNE (Branch Not Equal) instruction - branch taken
# x1=5, x2=10, branch to PC+4
li x1, 5
li x2, 10
bne x1, x2, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
nop            # This instruction should be skipped
li x1, 1       # Target of branch
ecall 0x3FF