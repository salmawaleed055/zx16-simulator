# TC-ZX16-10_BZ_Taken.s
# Test Objective: Validate BZ (Branch Zero) instruction - branch taken
# x1=0, branch to PC+4
li x1, 0
bz x1, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
nop       # This instruction should be skipped
li x1, 1  # Target of branch
ecall 0x3FF