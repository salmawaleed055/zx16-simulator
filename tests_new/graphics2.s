.org 0x0000
.text

main:
    li t0, 0x0000 # load base address of tile map into t0
    li t1, 0x012C # load base address of tile data into t1
    li s0, 0x092C # load base address of palette into s0

    li s1, 0x1C # load red color into s1
    sb s1, 0(s0) # store red color in palette

    li s1, 0x11 # ??? idk what this does
    sb s1, 0(t1) # store s1 in tile data -> set first tile as 0x11

    li s1, 0 # load 0 into s1
    sb s1, 0(t0) # store 0 in tile map base address -> set first in tile map to tile 0

    ecall 0x3FF  

    

