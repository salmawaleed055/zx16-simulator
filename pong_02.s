main:
    # === Setup Color Palette ===
    LUI x3, 500       # 0xFA00
    LI x4, 0          # Black
    SB x4, 0(x3)      # palette[0] = black
    LI x4, -1         # White (0xFF)
    SB x4, 1(x3)      # palette[1] = white

    # === Create White Block Tile (Tile 1) ===
    LUI x3, 484       # 0xF200
    ADDI x3, 63       # Skip to tile 1
    ADDI x3, 63
    ADDI x3, 2        # Now at 0xF280 (tile 1)

    # Fill tile 1 with white pixels
    LI x4, 17         # 0x11 = white pixels

    # [Keep all your existing tile filling code here - abbreviated for space]
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    # [Include all your remaining ADDI x3, 8 sections here...]

    # === Clear screen ===
    LUI x3, 480       # 0xF000 (tile map)
    LI x5, 0          # Black tile

    SB x5, 0(x3)
    SB x5, 1(x3)
    SB x5, 2(x3)
    SB x5, 3(x3)
    SB x5, 4(x3)
    SB x5, 5(x3)
    SB x5, 6(x3)
    SB x5, 7(x3)

    ADDI x3, 20
    SB x5, 0(x3)
    SB x5, 1(x3)
    SB x5, 2(x3)
    SB x5, 3(x3)
    SB x5, 4(x3)
    SB x5, 5(x3)
    SB x5, 6(x3)
    SB x5, 7(x3)

    # === Initialize game variables ===
    LI x0, 10         # Ball X position
    LI x1, 6          # Ball Y position
    LI x2, 3          # Right paddle Y position
    LI x7, 0          # Game collision flag

    # === Draw initial paddles ===
    LI x4, 1          # White tile

    # LEFT PADDLE - Column 0, Rows 3-8
    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60

    SB x4, 0(x3)      # Row 3
    ADDI x3, 20
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20
    SB x4, 0(x3)      # Row 5
    ADDI x3, 20
    SB x4, 0(x3)      # Row 6
    ADDI x3, 20
    SB x4, 0(x3)      # Row 7
    ADDI x3, 20
    SB x4, 0(x3)      # Row 8

    # RIGHT PADDLE - Draw at initial position
    LUI x3, 480       # Reset to tile map
    MV x5, x2         # Get paddle Y position (3)
    SLLI x5, 4        # y * 16 = 48
    ADD x3, x5        # Add to base
    MV x5, x2         # Copy Y again
    SLLI x5, 2        # y * 4 = 12
    ADD x3, x5        # Total: y * 20 = 60
    ADDI x3, 19       # Column 19

    SB x4, 0(x3)      # Draw 6 tiles
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

    # === KEYBOARD INPUT ===
    ECALL 7           # Read keyboard

    # Check if 'o' was pressed (move up)
    LI x3, 111        # ASCII 'o'
    BEQ x6, x3, do_move_up

    # Check if 'l' was pressed (move down)
    LI x3, 108        # ASCII 'l'
    BEQ x6, x3, do_move_down

    # No key - jump to ball animation
    J start_ball_animation

do_move_up:
    J move_paddle_up

do_move_down:
    J move_paddle_down

move_paddle_up:
    # Check boundary - use SHORT branch + jump pattern
    BNZ x2, paddle_up_ok    # Short branch
    J start_ball_animation  # Jump to distant target

paddle_up_ok:
    # Clear old paddle
    LUI x3, 480
    MV x5, x2
    SLLI x5, 4
    ADD x3, x5
    MV x5, x2
    SLLI x5, 2
    ADD x3, x5
    ADDI x3, 19

    LI x4, 0  # Black
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

    # Move paddle up
    ADDI x2, -1

    # Draw new paddle
    LUI x3, 480
    MV x5, x2
    SLLI x5, 4
    ADD x3, x5
    MV x5, x2
    SLLI x5, 2
    ADD x3, x5
    ADDI x3, 19

    LI x4, 1  # White
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

    J start_ball_animation

move_paddle_down:
    # Check boundary - use SHORT branch + jump pattern
    LI x3, 8
    BLT x2, x3, paddle_down_ok  # Short branch (opposite condition)
    J start_ball_animation      # Jump to distant target

paddle_down_ok:
    # Clear old paddle
    LUI x3, 480
    MV x5, x2
    SLLI x5, 4
    ADD x3, x5
    MV x5, x2
    SLLI x5, 2
    ADD x3, x5
    ADDI x3, 19

    LI x4, 0  # Black
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

    # Move paddle down
    ADDI x2, 1

    # Draw new paddle
    LUI x3, 480
    MV x5, x2
    SLLI x5, 4
    ADD x3, x5
    MV x5, x2
    SLLI x5, 2
    ADD x3, x5
    ADDI x3, 19

    LI x4, 1  # White
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

start_ball_animation:
    # === Ball Animation Sequence ===

    # === Frame 1: Ball at center ===
    LUI x3, 480       # 0xF000
    MV x5, x1         # Copy Y (6)
    SLLI x5, 4        # y * 16
    ADD x3, x5
    MV x5, x1         # Copy Y again
    SLLI x5, 2        # y * 4
    ADD x3, x5        # x3 = 0xF000 + y*20
    ADD x3, x0        # Add X position (10)

    LI x4, 1          # White tile
    SB x4, 0(x3)      # Draw ball

    # Long delay to see the ball
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

    # === Frame 2: Ball moves to (18, 4) - Test right paddle collision ===
    LI x0, 18         # Move ball to near right edge
    LI x1, 4          # Y position that should hit paddle if at default pos

    # Calculate new position
    LUI x3, 480       # 0xF000
    MV x5, x1         # Copy Y
    SLLI x5, 4        # y * 16
    ADD x3, x5
    MV x5, x1         # Copy Y again
    SLLI x5, 2        # y * 4
    ADD x3, x5        # x3 = 0xF000 + y*20
    ADD x3, x0        # Add X position

    LI x4, 1          # White tile
    SB x4, 0(x3)      # Draw ball

    # Check collision with right paddle (using dynamic position x2)
    MV x5, x2         # Get paddle start position
    SLT x6, x1, x5    # Check if ball Y < paddle start
    BNZ x6, ball_missed

    MV x5, x2         # Get paddle start again
    ADDI x5, 6        # Paddle ends after 6 tiles
    SLT x6, x1, x5    # Check if ball Y < paddle end
    BZ x6, ball_missed

    # Ball hit paddle!
    LI x7, 1          # Set collision flag
    J show_result

ball_missed:
    LI x7, 2          # Set miss flag

show_result:
    # Long delay
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0
    SB x4, 0(x3)

    # === Show result ===
    LUI x3, 480       # 0xF000
    LI x5, 1
    BEQ x7, x5, show_hit

    # Show miss at (0,0)
    LI x4, 1
    SB x4, 0(x3)
    J end_program

show_hit:
    # Show hit at (1,0)
    ADDI x3, 1
    LI x4, 1
    SB x4, 0(x3)

end_program:
    # End program
    ECALL 10