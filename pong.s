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
.equ PADDLE_LENGTH, 5
.equ TOP, 0 # max top where paddle can go up
.equ BOTTOM, 9  # (SCREEN_HEIGHT - PADDLE_LENGTH - 1)  max bottom where paddle can go down
.equ LEFT_X, 13
.equ RIGHT_X, 18

# BALL
.equ BALL_START_X, 10
.equ BALL_START_Y, 0
.equ BALL_LEFT, -1
.equ BALL_RIGHT, 1
.equ BALL_UP, -1
.equ BALL_DOWN, 1

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
    li s0, TILE_MAP_BASE
    j start
# ----------------------------------------------



# --- start the game ---
start:
    # set the y positions of paddles
    li t0, 3
    la t1, p1_paddle_y
    sw t0, 0(t1)

    la t1, p2_paddle_y
    sw t0, 0(t1)


    # set the position of ball
    li t0, BALL_START_X
    la t1, ball_x
    sw t0, 0(t1)

    li t0, BALL_START_Y
    la t1, ball_y
    sw t0, 0(t1)

    li t0, BALL_RIGHT
    la t1, ball_dx
    sw t0, 0(t1)

    li t0, BALL_DOWN
    la t1, ball_dy
    sw t0, 0(t1)

    li t0, 1
    la t1, ball_dy_wait
    sw t0, 0(t1)


    # load blue into palette
    lui s0, 18 # s0 = 0x092C
    ori s0, 0x2C
    li s1, 0x03 # blue
    sb s1, 4(s0) # palette[4] = 0x03

    # fill tile 0 with blue pixels (0x11)
    lui s0, 2 # s0 = 0x012C
    ori s0, 0x2C
    li s1, 0x44       
    li t1, 128 # tile size
    li t0, 0

    addi x0, 0
    addi x0, 0
    addi x0, 0
    addi x0, 0
    addi x0, 0
    addi x0, 0

    # fill_tile:
        # bge t0, t1, fill_done
        # sb s1, 0(s0)
        # addi s0, 1
        # addi t0, 1
        # j fill_tile

    #fill_done:
        # draw left paddle
        li a0, LEFT_X
        la t0, p1_paddle_y
        lw a1, 0(t0)
        li s1, 0

        draw_paddle_1:
            li t0, 20
            # Compute a1 * 20 using shifts
            sll t0, a1, 4       # t0 = y << 4 = y * 16
            sll t1, a1, 2       # t1 = y << 2 = y * 4
            add t0, t1      # t0 = y*16 + y*4 = y * 20
            add t0, a0      # t0 += x
            add t0, s1
            li s0, TILE_MAP_BASE
            add t0, s0 
            li t1, 0 # tile index 0
            sb t1, 0(t0)
            addi s1, 1
            li a1, PADDLE_LENGTH
            bne s1, a1, draw_paddle_1


        # draw right paddle
        li a0, RIGHT_X
        la t0, p2_paddle_y
        lw a1, 0(t0)
        li s1, 0
    
        draw_paddle_2:
            li t0, 20
            # Compute a1 * 20 using shifts
            sll t0, a1, 4       # t0 = y << 4 = y * 16
            sll t1, a1, 2       # t1 = y << 2 = y * 4
            add t0, t1      # t0 = y*16 + y*4 = y * 20
            add t0, a0      # t0 += x
            add t0, s1
            li s0, TILE_MAP_BASE
            add t0, s0 
            li t1, 0 # tile index 0
            sb t1, 0(t0)
            addi s1, 1
            li a1, PADDLE_LENGTH
            bne s1, a1, draw_paddle_2
            ecall 0x3FF


# --- clears board to prepare for new game ---
clear_game:
    li t0, TILE_MAP_BASE 
    li t1, BLANK_TILE
    lui s1, 2
    ori s1, 44 # load 300 into s1

    clear_loop:
        addi x0, 0
        addi x0, 0
        sb t1, 0(t0) # store blank tile at t[0]
        addi t0, 1 # t0++
        addi s1, -1 # t2--
        bge t1, s1, cleared
        j clear_loop
        cleared:
            addi x0, 0
            addi x0, 0
            ecall 0x3FF
# ----------------------------------------------
