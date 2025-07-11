#---------- CONSTANTS ----------
.equ TILE_MAP_BASE, 0x0000 # start of tile map buffer
.equ TILE_MAP_SIZE, 300 # 20 * 15 tiles
.equ BLANK_TILE, 0

.equ ASCII_W, 119
.equ ASCII_S, 115
.equ ASCII_O, 111
.equ ASCII_L, 108

# Movement flags
.equ STAY, 0
.equ UP,   1
.equ DOWN, 2

# PADDLES
.equ PADDLE_LENGTH, 2
.equ TOP, 0 # max top where paddle can go up
.equ BOTTOM, 9  # (SCREEN_HEIGHT - PADDLE_LENGTH - 1)  max bottom where paddle can go down
.equ LEFT_X, 5
.equ RIGHT_X, 18

# BALL
.equ BALL_START_X, 10
.equ BALL_START_Y, 0
.equ BALL_LEFT, -1
.equ BALL_RIGHT, 1
.equ BALL_UP, -1
.equ BALL_DOWN, 1

# TILE INDICES
.equ PADDLE_TILE, 1
.equ BALL_TILE, 2

.equ GAME_WIN, 10 # win when reach 10 points


#---------- DATA SECTION ----------
.data
p1_score: .word 0
p2_score: .word 0
p1_paddle_y: .word 7
p2_paddle_y: .word 7
ball_x: .word BALL_START_X
ball_y: .word BALL_START_Y
ball_dx: .word BALL_RIGHT
ball_dy: .word BALL_DOWN
ball_dy_wait:  .word 1# vertical speed counter


#---------- TEXT SECTION ----------
.org 0x0020
.text


# -- entry point for program ---
main:
    # Initialize stack pointer

# ----------------------------------------------

setup_graphics:
    lui s0, 18 # load palette base address
    ori s0, 0x2C

    li t1, 0x03 # load blue
    sb t1, 4(s0)

    li t1, 0xFF # load white
    sb t1, 7(s0)

    # fill tile 1 (paddle) with blue 
    lui t0, 2  # load tile data base address
    ori t0, 0x2C
    addi t0, 63 # Offset to tile 1 (128 bytes per tile)
    addi t0, 63 
    addi t0, 2

    # li t1, 0x44
    li t1, 0     # t1 = 0
    addi t1, 63  # t1 = 63
    addi t1, 5   # t1 = 68 (0x44)

    li s0, 128

    addi x0, 0
    fill_paddle:
        sb t1, 0(t0)
        addi t0, 1
        addi s0, -1
        bnz s0, fill_paddle

    draw_paddle:
        # draw left paddle
        li a0, LEFT_X
        la t0, p1_paddle_y
        lw a1, 0(t0)
        li t0, PADDLE_TILE
        addi sp, -2
        sw t0, 0(sp)

        lw s1, 0(sp) # tile index
        li s0, PADDLE_LENGTH

        draw_paddle_loop:
            addi x0, 0
            # li t0, 20
            # Compute a1 * 20 using shifts
            sll t0, a1, 4       # t0 = y << 4 = y * 16
            sll t1, a1, 2       # t1 = y << 2 = y * 4
            add t0, t1      # t0 = y*16 + y*4 = y * 20
            add t0, a0      # t0 += x
            li t1, TILE_MAP_BASE
            add t0, t0
            sb s1, 0(t1)

            addi a1, 1
            addi s0, -1
            li  ra, 0
            # bne s0, ra, draw_paddle_loop
            beq s0, ra, draw_ball
            j draw_paddle_loop

    draw_ball:
        # fill tile 2 (ball) with white
        addi x0, 0
        addi x0, 0
        addi x0, 0
        lui t0, 2
        ori t0, 0x2C
        addi t0, 128
        addi t0, 128
        # li t1, 0xFF
        li t1, 0     # t1 = 0
        addi t1, 63  # t1 = 63
        addi t1, 5   # t1 = 68 (0x44)
        li s0, 128

        addi x0, 0      
        fill_ball:
            addi x0, 0 
            addi x0, 0 
            addi x0, 0 
            sb t1, 0(t0)
            addi t0, 1
            addi s0, -1

            li ra, 0
            beq s0, ra, next
            j fill_ball

        next: 
            add x0, 0
            add x0, 0
            la t0, ball_x
            lw a0, 0(t0)
            la t0, ball_y
            lw a1, 0(t0)

            sll t0, a1, 4
            sll t1, a1, 2
            add t0, t1
            add t0, a0
            add t0, t0

            li t1, TILE_MAP_BASE
            add t0, t1

            li t1, BALL_TILE
            sb t1, 0(t0)


            ecall 0x3FF














# # --- start the game ---
# start:
    

# # --- clears board to prepare for new game ---
# clear_game:
#     li t0, TILE_MAP_BASE 
#     li t1, BLANK_TILE
#     lui s1, 2
#     ori s1, 44 # load 300 into s1

#     clear_loop:
#         addi x0, 0
#         addi x0, 0
#         sb t1, 0(t0) # store blank tile at t[0]
#         addi t0, 1 # t0++
#         addi s1, -1 # t2--
#         bge t1, s1, cleared
#         j clear_loop
#         cleared:
#             addi x0, 0
#             addi x0, 0
#             ecall 0x3FF
# # ----------------------------------------------
