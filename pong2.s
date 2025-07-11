.data
.org 0xF000
# -------- TILE MAP (15 R x 20 C) --------
Row0:  .fill 20, 1, 0
Row1:  .fill 20, 1, 0
Row2:  .fill 20, 1, 0
Row3:  .fill 20, 1, 0
Row4:  .fill 20, 1, 0
Row5:  .fill 20, 1, 0
Row6:  .fill 20, 1, 0
Row7:  .fill 20, 1, 0
Row8:  .fill 20, 1, 0
Row9:  .fill 20, 1, 0
Row10: .fill 20, 1, 0
Row11: .fill 20, 1, 0
Row12: .fill 20, 1, 0
Row13: .fill 20, 1, 0
Row14: .fill 20, 1, 0
# ------------------------------------------


# -------- GAME VARIABLES --------
score1: .byte 0  # Player 1 score
score2: .byte 0 # Player 2 score
p1_position: .word 0x0708 # Player 1 position (x=7, y=8)
p2_position: .word 0x0C08 # Player 2 position (x=12, y=8)
ball_position: .word 0x0A08 # Ball position (x=10, y=8)
ball_direction: .word 0x01FF # Ball direction (dx=1, dy=-1)
# --------------------------------


# -------- TILE DEFINITIONS --------
.org 0xF200
Tile0: .fill 128, 1, 0x00   # Blank (black)
Tile1: .fill 128, 1, 0x11   # Blue paddle (color index 1)
Tile2: .fill 128, 1, 0x22   # White ball (color index 2)
Tile3: .fill 128, 1, 0x00   # Empty
Tile4: .fill 128, 1, 0x00
Tile5: .fill 128, 1, 0x00
Tile6: .fill 128, 1, 0x00
Tile7: .fill 128, 1, 0x00
Tile8: .fill 128, 1, 0x00
Tile9: .fill 128, 1, 0x00
Tile10:.fill 128, 1, 0x00
Tile11:.fill 128, 1, 0x00
Tile12:.fill 128, 1, 0x00
Tile13:.fill 128, 1, 0x00
Tile14:.fill 128, 1, 0x00
Tile15:.fill 128, 1, 0x00
-------------------------------------


# -------- COLOR PALETTTE --------
.org 0xFA00
.byte 0x00   # Color 0: Black (000000)
.byte 0x03   # Color 1: Blue (00000011)
.byte 0xFF   # Color 2: White (11111111)
.fill 13, 1, 0x00   # Unused colors
----------------------------------


# ----------------  TEXT SECTION ----------------
.org 0x0020
.text

main:
    li sp, 0xEFFE

    li t1, 1
    lui t0, 0xF
    ori t0, 0x0000
    sb t1, 0(t0)

draw_paddles:
    # Draw paddle 1
    li t0, p1_position
    lw s0, 0(t0)
    
    mv a0, s0
    andi a0, 0x00FF
    
    mv a1, s0
    srli a1, 8


    li s1, -1

    draw_paddle_loop:
        add a0, s1 
        li t1, 20

        mv t0, a0
        slli t0, 4

        mv s0, a0
        slli s0, 2
        add t0, s0
        add t0, a1

        lui a1, 0xF
        ori a1, 0x0000
        add t0, a1

        li a1, 1
        sb a1, 0(t0)

        addi s1, 1
        li t0, 2
        bne s1, t0, draw_paddle_loop
        ecall 0x3FF









    lw a1, 1(t0) # Y position
    mv a0, a1

    addi a0, -1 # Top
    li t1, 7
    li t0, 1

    mv t0, a0
    slli a1, t0, 4
    slli t0, t0, 2
    add t0, a1
    addi t0, t1

    li16 a1, 0xF000
    add t0, a1
    
    sb ra, 0(t0)

    jr ra



    


