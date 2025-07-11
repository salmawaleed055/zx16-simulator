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
.equ PADDLE_LENGTH, 5
.equ PADDLE_SPEED, 1
.equ TOP_PADDLE_Y, 0
.equ BOTTOM_PADDLE_Y, 14
.equ PADDLE_MIN_X, 2
.equ PADDLE_MAX_X, 17
.equ BALL_START_X, 10
.equ BALL_START_Y, 7

# Key codes
.equ KEY_W, 119
.equ KEY_S, 115
.equ KEY_O, 111
.equ KEY_L, 108
# ------------------------


# ------ Game state addresses ------
.equ P1_X_ADDR, 0xF12C
.equ P2_X_ADDR, 0xF12E
.equ BALL_X_ADDR, 0xF130
.equ BALL_Y_ADDR, 0xF132
# ------------------------


# ------ GAME STATE ------
.data
p1_x: .word 9  # Player 1 paddle X position
p2_x: .word 7   # Player 2 paddle Y position
ball_x: .word BALL_START_X
ball_y: .word BALL_START_Y
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
    addi t0, 63 # 256
    addi t0, 63
    addi t0, 4
    addi t0, 63
    addi t0, 63

    li t1, 0x22
    addi s0, 63 # 128
    addi s0, 63
    addi s0, 2

    addi x0, 0
    init_ball_tile:
        sb t1, 0(t0)
        addi t0, 1
        addi s0, -1
        bnz s0, init_ball_tile


game_loop:
    # Handle input for player 1 (W/S)
        li a0, KEY_W
        ecall 7

        bz a1, check_s
        lw t0, 0(P1_X_ADDR)
        li t1, PADDLE_MIN_X
        blt t0, t1, check_s #if at min, skip movement
        addi t0, -1
        sw t0, 0(P1_X_ADDR)
        j update_display

        check_s:
            li a0, KEY_S
            ecall 7

            bz a1, update_display
            lw t0, 0(P1_X_ADDR)
            li t1, PADDLE_MAX_X
            bge t0, t1, update_display # if at max, skip movement
            addi t0, 1
            sw t0, 0(P1_X_ADDR)


        update_display:
            # clear screen 
            li t0, TILE_MAP_BASE
            li s1, SCREEN_WIDTH
            li s0, SCREEN_HEIGHT

            mv t1, s1
            slli t1, 4

            sub t1, s1
            mv s1, t1

            li t1, EMPTY_TILE

            add x0, 0
            clear_loop:
                sb t1, 0(t0)
                addi t0, 1
                addi s1, -1
                bnz s1, clear_loop


            # Draw paddle 1
            lw a0, 0(P1_X_ADDR)
            li a1, TOP_PADDLE_Y
            li s1, PADDLE_TILE

            # calculate starting address

            mv t0, a1
            mv t1, a1
            slli t0, 4
            slli t1, 2
            add t0, t1

            mv t1, a0
            addi t1, -2
            add t0, t1 

            li t1, TILE_MAP_BASE 

            add t0, t1 

            li s0, PADDLE_LENGTH

            addi x0, 0
            draw_p1:
                sb s1, 0(t0)
                addi t0, 1
                addi s0, -1
                bnz s0, draw_p1


            j game_loop











    # draw_paddles:
    #     # player 1
    #     li a0, TOP_PADDLE_Y  # 1
    #     lw a1, 0(p1_x) # word 7
    #     li s1, PADDLE_TILE
    #     li t0, TILE_MAP_BASE

    #     mv s0, a1
    #     slli s0, 4

    #     mv t1, a1
    #     slli t1, 2

    #     add t1, s0
    #     add t1, a0
    #     add t1, t0

    #     li s0, PADDLE_LENGTH
    #     add x0, 0
    #     draw_paddle1_loop:
    #         sb s1, 0(t1)
    #         addi t1, 1
    #         addi a1, 1
    #         addi s0, -1
    #         bnz s0, draw_paddle1_loop

    #     # player 2
    #     li t0, 0
    #     li t1, 0
    #     li s0, 0
    #     li s1, 0
    #     li a0, 0
    #     li a1, 0

    #     li a0, BOTTOM_PADDLE_Y 
    #     lw a1, 0(p2_x) 
    #     li s0, PADDLE_LENGTH
    #     li s1, PADDLE_TILE
    #     li t0, TILE_MAP_BASE

    #     mv s0, a1
    #     slli s0, 4

    #     mv t1, a1
    #     slli t1, 2

    #     add t1, s0
    #     add t1, a0
    #     add t1, t0


    #     li s0, SCREEN_WIDTH # 20
    #     add x0, 0
    #     draw_paddle2_loop:
    #         sb s1, 0(t1)
    #         addi t1, 1
    #         addi a1, 1
    #         addi s0, -1
    #         bnz s0, draw_paddle2_loop


    # draw_ball:
    #     li t0, 0
    #     li t1, 0
    #     li s0, 0
    #     li s1, 0
    #     li a0, 0
    #     li a1, 0

    #     la t0, ball_x
    #     lw a0, 0(t0)
    #     la t0, ball_y
    #     lw a1, 0(t0)

    #     li t0, 20

    #     mv t1, a1
    #     slli t1, 4

    #     mv s0, a1
    #     slli s0, 2

    #     add t1, s0
    #     add t1, a0

    #     li t0, TILE_MAP_BASE
    #     add t1, t0 

    #     li s1, BALL_TILE
    #     sb s1, 0(t1)
        
    #     ecall 0x3FF



