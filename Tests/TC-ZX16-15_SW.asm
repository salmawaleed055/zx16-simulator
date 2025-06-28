# TC-ZX16-15_SW.s
# Test Objective: Validate SW (Store Word) instruction (little-endian)
# mem[a0-2] = x1[15:0]
li x1, 0x1234 # Data for SW: x1 = 0x1234
li a0, 0x1002 # Base address: a0 = 0x1002 (target 0x1000)
sw x1, -2(a0) # Store word 0x1234 from x1 at mem[0x1002 - 2] = mem[0x1000]
              # Expected: mem[0x1000]=0x34, mem[0x1001]=0x12
ecall 0x3FF