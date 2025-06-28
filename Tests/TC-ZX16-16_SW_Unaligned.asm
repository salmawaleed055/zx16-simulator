# TC-ZX16-16_SW_Unaligned.s
# Test Objective: Validate SW (Store Word) instruction with unaligned address
# Should report an error and not write to memory.
li x1, 0xDEAD # Data to attempt to store
li a0, 0x1002 # Base address: a0 = 0x1002
sw x1, -1(a0) # Attempt to store at mem[0x1002 - 1] = mem[0x1001] (unaligned)
ecall 0x3FF