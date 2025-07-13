# --------- CONSTANTS ---------
.equ TILE_MAP_BASE, 0xF000
.equ TILE_SET_BASE, 0xF200
.equ PALETTE_BASE,  0xFA00

.equ COLS, 20
.equ ROWS, 15
.equ PADDLE_H, 3

.equ EMPTY_TILE, 0
.equ WALL_TILE, 1
.equ PADDLE_TILE, 2
.equ BALL_TILE, 3

.equ VARS, 0x0100
#-----------------------------
.data
.org 0x0020

initial_screen:
    .byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1


.org 0xF200
empty_tile: .fill 128, 1, 0x00

wall_tile: .fill 128, 1, 0x77

paddle_tile: .fill 128, 1, 0x44

ball_tile:
    .fill 16, 1, 0x00
    .byte   0x00,0x00,0x77,0x77,0x00,0x00,0x00,0x00
    .byte   0x00,0x77,0x77,0x77,0x77,0x00,0x00,0x00
    .byte   0x07,0x77,0x77,0x77,0x77,0x70,0x00,0x00
    .byte   0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00
    .byte   0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00
    .byte   0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00
    .byte   0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00
    .byte   0x07,0x77,0x77,0x77,0x77,0x70,0x00,0x00
    .byte   0x00,0x77,0x77,0x77,0x77,0x00,0x00,0x00
    .byte   0x00,0x00,0x77,0x77,0x00,0x00,0x00,0x00
    .fill   48, 1, 0x00

.space 1536 # 128 * 12


.org 0xFA00
palette:
    .byte   0x00            # 0: Black
    .byte   0xFF            # 1: White  
    .byte   0x1C            # 2: Red
    .byte   0x03            # 3: Blue
    .byte   0xE0            # 4: Green
    .byte   0xFC            # 5: Yellow
    .byte   0x1F            # 6: Magenta
    .byte   0xE3            # 7: Cyan
    .space  8               # Unused palette
# -------------------------------------------------


.text
.org 0x0020

main:
    # li16 sp, 0xEFFE
    lui sp, 0x1DF
    ori sp, 0x7E

# ------------------------------
init_game:
    # li16 a0, TILE_MAP_BASE
    lui a0, 0x78
    ori a0, 0x00

    # la a1, initial_screen
    lui a1, 0x01
    ori a1, 0x20

    # li t0, 300
    lui t0, 0x02
    ori t0, 0x2C

    addi x0, 0
    addi x0, 0
    copy_screen:
        lb t1, 0(a1)
        sb t1, 0(a0)
        addi a0, 1
        addi a1, 1
        addi t0, -1
        bnz t0, copy_screen

        # Initialize game variables
        li t0, 10

        # li16 a0, VARS
        lui a0, 0x02
        ori a0, 0x00


        sb t0, 0(a0) # ball x

        li t0, 7
        sb t0, 1(a0) # ball y

        li t0, 1
        sb t0, 2(a0) # ball dx
        sb t0, 3(a0) # ball dy

        li t0, 6
        sb t0, 4(a0) # paddle y

        # Clear old positions
        sb t0, 5(a0) # old ball x
        sb t0, 6(a0) # old ball y

    
    draw_paddle:
        li16 a0, VARS
        lb t0, 4(a0) # paddle y
        li t1, PADDLE_H
        li s0, 0

        add x0, 0
        paddle_loop:
            mv a0, s0
            mv a1, t0

            call calc_tile_address

            li s1, PADDLE_TILE
            sb s1, 0(a0)

            # next row 
            addi t0, 1
            addi t1, -1
            bnz t1, paddle_loop

    
    draw_ball:
        li16 a0, VARS
        lb t0, 0(a0) # ball x
        lb t1, 1(a0) # ball y

        mv a0, t0
        mv a1, t1

        call calc_tile_address

        li t0, BALL_TILE
        sb t0, 0(a0)

calc_tile_address:
    li16 a0, TILE_MAP_BASE
    li s1, 20

    # multiply row by 20
    mv t1, a1
    sll t1, 4
    
    mv t0, a1
    sll t0, 2

    add t1, t0

    add s0, t1

    add a0, s0

    ret 


# ------- GAME LOOP -------
game_loop:
    call handle_input
    call update_ball
    call render_changes
    call delay

    j game_loop
# ------------------------


handle_input:

update_ball:

render_changes:

delay:
