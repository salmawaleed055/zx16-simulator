main:
    # === Setup Color Palette ===
    LUI x3, 500       # 0xFA00
    LI x4, 0          # Black
    SB x4, 0(x3)      # palette[0] = black
    LI x4, -1         # White (0xFF)
    SB x4, 1(x3)      # palette[1] = white

    LUI x3, 484       # 0xF200
    ADDI x3, 63       # Skip to tile 1
    ADDI x3, 63
    ADDI x3, 2        # Now at 0xF280 (tile 1)

    LI x4, 17         # 0x11 = white pixels
    LI x5, 32

# === NOW SHOW PADDLES ON CLEAR SCREEN ===

    LI x4, 1          # White tile


    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60
    ADDI x3, 0        # Column 0 (leftmost)

    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 7
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 8

    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60
    ADDI x3, 19       # Column 19 (rightmost)

    # Draw right paddle (6 tiles vertical)
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 7
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 8
fill_tile:
    SB x4, 0(x3)
    ADDI x3, 1
    ADDI x5, -1
    BNZ x5, fill_tile

    LUI x3, 480       # 0xF000 (tile map)
    LI x5, 0          # Black tile
    LI x4, 300        # Clear 300 tiles
clear_screen:
    SB x5, 0(x3)
    ADDI x3, 1
    ADDI x4, -1
    BNZ x4, clear_screen

    LI x0, 10         # Ball X position
    LI x1, 6          # Ball Y position
    LI x2, 1          # Ball X velocity
    LI x3, 3          # Right paddle Y position
    LI x6, 0          # Left score
    LI x7, 0          # Right score

    LI x4, 1          # White tile
    LUI x5, 480       # 0xF000
    ADDI x5, 60       # Row 3
    SB x4, 0(x5)
    ADDI x5, 20
    SB x4, 0(x5)
    ADDI x5, 20
    SB x4, 0(x5)
    ADDI x5, 20
    SB x4, 0(x5)
    ADDI x5, 20
    SB x4, 0(x5)
    ADDI x5, 20
    SB x4, 0(x5)

    # Draw initial right paddle
    LUI x4, 480
    MV x5, x3
    SLLI x5, 4
    ADD x4, x5
    MV x5, x3
    SLLI x5, 2
    ADD x4, x5
    ADDI x4, 19

    LI x5, 1
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)

# === MAIN GAME LOOP ===
g
game_loop:
    li x6, 440      # Frequency for sound
   li x7, 100      # Duration
   ecall 4         # SYS_TONE
    # === Check for keyboard input ===
    ECALL 7           # Read keyboard - key in x6 (a0), pressed flag in x7 (a1)

    # Check if a key was pressed
    BZ x7, no_key_pressed    # If no key pressed, skip input handling

    # Check for right paddle controls
    LI x5, 111        # ASCII code for 'o' (move up)
    BEQ x6, x5, move_right_paddle_up

    LI x5, 108        # ASCII code for 'l' (move down)
    BEQ x6, x5, move_right_paddle_down

    J no_key_pressed  # If not our keys, continue

move_right_paddle_up:
    # Check if paddle can move up (top row > 0)
    BZ x3, no_key_pressed   # If already at top, don't move

    # Clear old paddle position
    CALL clear_right_paddle

    # Move paddle up
    ADDI x3, -1

    # Draw new paddle position
    CALL draw_right_paddle

    J no_key_pressed

move_right_paddle_down:
    # Check if paddle can move down (bottom row < 9)
    # Paddle is 6 tiles tall, so bottom row = top row + 5
    MV x5, x3         # Copy paddle position
    ADDI x5, 5        # Calculate bottom row
    LI x6, 9          # Maximum bottom row
    BGE x5, x6, no_key_pressed  # If would go past bottom, don't move

    # Clear old paddle position
    CALL clear_right_paddle

    # Move paddle down
    ADDI x3, 1

    # Draw new paddle position
    CALL draw_right_paddle

no_key_pressed:
    # [Continue with your existing ball movement code...]

    # Add some delay
    NOP
    NOP
    NOP
    NOP

    J game_loop       # Loop back to check for more input

# Helper function to draw right paddle
draw_right_paddle:
    # Draw right paddle at current position (x3)
    LI16 x5, 0xF000   # Tile map base
    MV x6, x3         # Get paddle Y position

    # Calculate y * 20 for row offset
    SLLI x6, 4        # y * 16
    MV x7, x3         # Copy Y again
    SLLI x7, 2        # y * 4
    ADD x6, x7        # y * 20
    ADD x5, x6        # Add to base address
    ADDI x5, 19       # Column 19 (rightmost)

    LI x4, 1          # White tile

    # Draw 6 tiles vertically
    SB x4, 0(x5)      # Row 0
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 1
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 2
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 3
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 4
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 5

    RET

# Helper function to clear right paddle
clear_right_paddle:
    # Clear right paddle at current position (x3)
    LI16 x5, 0xF000   # Tile map base
    MV x6, x3         # Get paddle Y position

    # Calculate y * 20 for row offset
    SLLI x6, 4        # y * 16
    MV x7, x3         # Copy Y again
    SLLI x7, 2        # y * 4
    ADD x6, x7        # y * 20
    ADD x5, x6        # Add to base address
    ADDI x5, 19       # Column 19 (rightmost)

    LI x4, 0          # Black tile

    # Clear 6 tiles vertically
    SB x4, 0(x5)      # Row 0
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 1
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 2
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 3
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 4
    ADDI x5, 20       # Next row
    SB x4, 0(x5)      # Row 5

    RET
    # Check for keyboard input
    ECALL 7           # Read character

    # Check 'w' key
    LI x4, 119
    BEQ x6, x4, try_up

    # Check 's' key
    LI x4, 115
    BEQ x6, x4, try_down

    J ball_update

try_up:
    # Check if can move up
    BZ x3, ball_update
    J move_up

try_down:
    # Check if can move down
    LI x4, 9
    BEQ x3, x4, ball_update
    J move_down

move_up:
    # Clear old paddle
    LUI x4, 480
    MV x5, x3
    SLLI x5, 4
    ADD x4, x5
    MV x5, x3
    SLLI x5, 2
    ADD x4, x5
    ADDI x4, 19

    LI x5, 0
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)

    # Move paddle up
    ADDI x3, -1
    J draw_paddle

move_down:
    # Clear old paddle
    LUI x4, 480
    MV x5, x3
    SLLI x5, 4
    ADD x4, x5
    MV x5, x3
    SLLI x5, 2
    ADD x4, x5
    ADDI x4, 19

    LI x5, 0
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)

    # Move paddle down
    ADDI x3, 1

draw_paddle:
    # Draw paddle at new position
    LUI x4, 480
    MV x5, x3
    SLLI x5, 4
    ADD x4, x5
    MV x5, x3
    SLLI x5, 2
    ADD x4, x5
    ADDI x4, 19

    LI x5, 1
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)
    ADDI x4, 20
    SB x5, 0(x4)

ball_update:
    # Clear ball
    LUI x4, 480
    MV x5, x1
    SLLI x5, 4
    ADD x4, x5
    MV x5, x1
    SLLI x5, 2
    ADD x4, x5
    ADD x4, x0

    LI x5, 0
    SB x5, 0(x4)

    # Update ball position
    ADD x0, x2
    ADDI x1, 1

    # Y bounds
    LI x4, 14
    BEQ x1, x4, reset_y_top
    BZ x1, reset_y_bottom
    J check_x

reset_y_top:
    LI x1, 0
    J check_x

reset_y_bottom:
    LI x1, 14

check_x:
    # Check left collision
    LI x4, 1
    BEQ x0, x4, left_collision

    # Check right collision
    LI x4, 18
    BEQ x0, x4, right_collision

    # Check scoring
    BZ x0, right_point
    LI x4, 19
    BEQ x0, x4, left_point

    J draw_new_ball

left_collision:
    # Check if hits left paddle (rows 3-8)
    LI x4, 3
    SLT x4, x1, x4
    BNZ x4, right_point
    LI x4, 9
    SLT x4, x1, x4
    BZ x4, right_point
    # Hit paddle
    LI x2, 1
    J draw_new_ball

right_collision:
    # Check if hits right paddle
    SLT x4, x1, x3
    BNZ x4, left_point
    MV x4, x3
    ADDI x4, 6
    SLT x4, x1, x4
    BZ x4, left_point
    # Hit paddle
    LI x2, -1
    J draw_new_ball

right_point:
    ADDI x7, 1
    J check_win

left_point:
    ADDI x6, 1

check_win:
    LI x4, 3
    BEQ x6, x4, left_win
    BEQ x7, x4, right_win
    # Reset ball
    LI x0, 10
    LI x1, 6
    LI x2, 1

draw_new_ball:
    # Draw ball
    LUI x4, 480
    MV x5, x1
    SLLI x5, 4
    ADD x4, x5
    MV x5, x1
    SLLI x5, 2
    ADD x4, x5
    ADD x4, x0

    LI x5, 1
    SB x5, 0(x4)

    # Delay
    LI x4, 8
delay:
    ADDI x4, -1
    BNZ x4, delay

    J game_loop

left_win:
    LUI x4, 480
    ADDI x4, 200
    LI x5, 1
    SB x5, 0(x4)
    J end_game

right_win:
    LUI x4, 480
    ADDI x4, 205
    LI x5, 1
    SB x5, 0(x4)

end_game:

    # Clear LEFT PADDLE - Column 0, Rows 3-8
    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60
    ADDI x3, 0        # Column 0 (leftmost)

    # Clear left paddle (6 tiles vertical)
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 7
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 8

    # Clear RIGHT PADDLE - Column 19, Rows 3-8
    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60
    ADDI x3, 19       # Column 19 (rightmost)

    # Clear right paddle (6 tiles vertical)
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 7
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 8
  j main
    ECALL 10