.org 0x0020
.text

main:
    # Palette base address 0x092C
    lui s0, 0x12
    ori s0, 0x2C
    li s1, 0x1C      # red color palette entry 1 (0x1C)
    sb s1, 1(s0)     # set palette[1] = red

    # Tile data base 0x012C
    lui t1, 0
    ori t1, 0x012C

    li s1, 0x11      # tile pixel pair with palette index 1
    li s0, 128       # 128 bytes per tile
    li t0, 0

tile_loop:
    bge t0, s0, tile_done
    sb s1, 0(t1)
    addi t1, t1, 1
    addi t0, t0, 1
    j tile_loop

tile_done:
    # Tile map base 0x0000
    li t1, 0x0000
    li s1, 0x00      # tile index 0
    sb s1, 0(t1)     # set tile map position 0 = tile 0

    ecall 0x3FF      # exit
