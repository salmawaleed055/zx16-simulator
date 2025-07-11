# ------ CONSTANTS ------
.equ TILE_MAP_BASE, 0x0000
.equ PALETTE_BASE, 0x092C
.equ TILE_DATA_BASE, 0x012C
.equ SCREEN_WIDTH, 20
.equ SCREEN_HEIGHT, 15

# Tile indices
.equ EMPTY_TILE, 0
.equ PADDLE_TILE, 1
.equ BALL_TILE, 2

# Game constants
.equ PADDLE_LENGTH, 5
.equ PADDLE_SPEED, 1
.equ BALL_SPEED, 1
.equ LEFT_PADDLE_X, 7
.equ RIGHT_PADDLE_X, 18
.equ PADDLE_MIN_Y, 1
.equ PADDLE_MAX_Y, 12

# Key codes
.equ KEY_W, 119
.equ KEY_S, 115
.equ KEY_O, 111
.equ KEY_L, 108
# ------------------------


# ------ GAME STATE ------
.data
p1_y: .word 10  # Player 1 paddle Y position
p2_y: .word 7   # Player 2 paddle Y position
ball_x: .word 7
ball_y: .word 1
ball_dx: .word 1
ball_dy: .word 1
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

    
    # ball tile (white)
    li t0, TILE_DATA_BASE
    addi t0, 63
    addi t0, 63
    addi t0, 2
    addi t0, 63
    addi t0, 63
    addi t0, 2

    li t1, 0x22
    addi s0, 63
    addi s0, 63
    addi s0, 2

    addi x0, 0
    init_ball_tile:
        sb t1, 0(t0)
        addi t0, 1
        addi s0, -1
        bnz s0, init_ball_tile

    draw_paddles:
        # player 1
        li a0, LEFT_PADDLE_X  # 1
        lw a1, 0(p1_y) # word 7

        draw_paddle:
            li t0, TILE_MAP_BASE
            li t1, SCREEN_WIDTH # 20
            li t1, 140

            add t1, a0
            add t0, t1

            li s0, PADDLE_LENGTH
            li s1, PADDLE_TILE

            draw_paddle_loop:
                sb s1, 0(t0)
                addi t0, SCREEN_WIDTH
                addi s1, -1
                bnz s1, draw_paddle_loop
                

    draw_ball:
        la t0, ball_x
        lw a0, 0(t0)
        la t0, ball_y
        lw a1, 0(t0)

        li t0, 20
        sll t1, a1, 4
        sll s0, a1, 2
        add t1, s0

        add t1, a0
        li t0, TILE_MAP_BASE
        add t1, t0 

        li t0, BALL_TILE
        sb t0, 0(t1)

        ecall 0x3FF




