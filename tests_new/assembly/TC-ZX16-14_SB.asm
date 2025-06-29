# TC-ZX16-14_SB.s
# Test Objective: Validate SB (Store Byte) instruction
# mem[a0+2] = x1[7:0]
li x1, 0x0105 # Data for SB: x1 = 0x0105 (byte to store is 0x05)
li a0, 0x1000 # Base address: a0 = 0x1000
sb x1, 2(a0)  # Store byte 0x05 from x1 at mem[0x1000 + 2] = mem[0x1002]
ecall 0x3FF