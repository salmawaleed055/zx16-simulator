# TC-ZX16-12_BGEU_UnsignedTaken.s
# Test Objective: Validate BGEU (Branch Greater Than or Equal Unsigned) instruction - branch taken
# x1=-5 (0xFFFB), x2=10 (0x000A), branch to PC+4 (0xFFFB >= 0x000A unsigned is true)
li x1, -5  # x1 = 0xFFFB
li x2, 10  # x2 = 0x000A
bgeu x1, x2, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
nop        # Skipped
li x1, 1   # Target
ecall 0x3FF