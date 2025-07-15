# ------ CONSTANTS ------
.equ TILE_MAP_BASE, 0xF000
.equ PALETTE_BASE, 0xFA00
.equ TILE_DATA_BASE, 0xF200
.equ SCREEN_WIDTH, 20
.equ SCREEN_HEIGHT, 15

# Tile indices
.equ EMPTY_TILE, 0
.equ PADDLE_TILE, 1
.equ BALL_TILE, 2

# Game constants
.equ PADDLE_LENGTH, 3
.equ PADDLE_SPEED, 1
.equ BALL_SPEED, 1
.equ LEFT_PADDLE_X, 8
.equ RIGHT_PADDLE_X, 8
.equ PADDLE_MIN_X, 1
.equ PADDLE_MAX_X, 17

# Key codes
.equ KEY_W, 119
.equ KEY_S, 115
.equ KEY_O, 111
.equ KEY_L, 108
# ------------------------


# ------ Game state addresses ------
.equ P1_Y_ADDR, 0xF12C
.equ P2_Y_ADDR, 0xF12E
.equ BALL_X_ADDR, 0xF130
.equ BALL_Y_ADDR, 0xF132
.equ BALL_DX_ADDR, 0xF134
.equ BALL_DY_ADDR, 0xF136
# ------------------------


# ------ GAME STATE ------
.data
p1_y: .word 7  # Player 1 paddle Y position
p2_y: .word 7   # Player 2 paddle Y position
ball_x: .word 10
ball_y: .word 7
ball_dx: .word 1
ball_dy: .word -1
# --------------------------


# ------ TEXT SECTION ------
.org 0x0020
.text

main:
    # initialize stack pointer
    li sp, 0xEFFE


init_graphics:
    # Set up palette
    li t0, PALETTE_BASE
    li t1, 0x00     # Color 0: Black (background)
    sb t1, 0(t0)
    li t1, 0x03     # Color 1: Blue (paddle)
    sb t1, 1(t0)
    li t1, 0xFF     # Color 2: White (ball)
    sb t1, 2(t0)


    # paddle tile (blue)
    li t0, TILE_DATA_BASE
    addi t0, 63
    addi t0, 63
    addi t0, 2

    li t1, 0x11
    addi s0, 63
    addi s0, 63
    addi s0, 2

    addi x0, 0
    init_paddle_tile:
        sb t1, 0(t0)
        addi t0, 1
        addi s0, -1
        bnz s0, init_paddle_tile

    draw_paddles:
        # player 1
        li t0, TILE_MAP_BASE
        li a0, LEFT_PADDLE_X # 7
        add t0, a0
        mv t1, t0

        li s1, PADDLE_TILE
        sb s1, 0(t1)

        addi t1, 1
        sb s1, 0(t1)

        addi t1, 1
        sb s1, 0(t1)


        # player 2
        li t0, TILE_MAP_BASE
        li a0, 63
        addi a0, 63
        addi a0, 63
        addi a0, 63
        addi a0, 28

        add t0, a0

        li a0, RIGHT_PADDLE_X
        add t0, a0
        mv t1, t0

        li s1, PADDLE_TILE
        sb s1, 0(t1)

        addi t1, 1
        sb s1, 0(t1)

        addi t1, 1
        sb s1, 0(t1)



    ecall 0x000 
    ecall 0x3FF  


    # end