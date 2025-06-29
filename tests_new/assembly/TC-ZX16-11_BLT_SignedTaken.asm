# TC-ZX16-11_BLT_SignedTaken.s
# Test Objective: Validate BLT (Branch Less Than Signed) instruction - branch taken
# x1=-5, x2=10, branch to PC+4 (-5 < 10 is true)
li x1, -5  # x1 = 0xFFFB
li x2, 10  # x2 = 0x000A
blt x1, x2, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
nop        # Skipped
li x1, 1   # Target
ecall 0x3FF