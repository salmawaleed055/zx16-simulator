# TC-ZX16-13_BLTU_UnsignedNotTaken.s
# Test Objective: Validate BLTU (Branch Less Than Unsigned) instruction - branch not taken
# x1=5, x2=10, branch to PC+2 (5 < 10 unsigned is true, but we expect NOT taken for this specific test goal)
# The current logic will take the branch if true. To make it "not taken" for test goal, we need (5 < 10) to be false.
# Let's reverse the condition to make it not taken. e.g. x1=10, x2=5
# x1=10, x2=5, branch to PC+2 (10 < 5 unsigned is false, so not taken)
li x1, 10 # x1 = 0x000A
li x2, 5  # x2 = 0x0005
bltu x1, x2, +4 # Current PC + 4 = 0x0004 + 4 = 0x0008
li x1, 1  # Executed
nop       # Now at PC 0x0008
ecall 0x3FF