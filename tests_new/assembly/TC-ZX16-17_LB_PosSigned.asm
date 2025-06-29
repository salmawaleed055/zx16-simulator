# TC-ZX16-17_LB_PosSigned.s
# Test Objective: Validate LB (Load Byte, signed-extended) with a positive byte value
# x0 = sext(mem[a1+2][7:0]) => sext(0xCC) = 0xFFCC
li a1, 0x2000 # Base address
# (Memory needs to be pre-set by the .bin file for this test)
# Binary will contain the instruction, then data at 0x2002: 0xCC
lb x0, 2(a1)  # Load byte from mem[0x2000 + 2] = mem[0x2002]
ecall 0x3FF