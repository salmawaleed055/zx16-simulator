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

       # Fill tile 1 with white pixels (simplified approach like original working code)
       LI x4, 17         # 0x11 = white pixels

       # Fill enough bytes to make tile visible (like the working white stripes)
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

       ADDI x3, 8
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

   ADDI x3, 8
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

   ADDI x3, 8
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

   ADDI x3, 8
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

   ADDI x3, 8
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

   ADDI x3, 8
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
   ADDI x3, 8
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
   ADDI x3, 8
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
   ADDI x3, 8
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
   ADDI x3, 8
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
   ADDI x3, 8
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
   ADDI x3, 8
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


       # === Clear screen and place white tiles directly ===
       LUI x3, 480       # 0xF000 (tile map)
       LI x4, 1          # White tile (tile 1)

       # Create visible pattern by placing white tiles
       # First, clear some area
       LI x5, 0          # Black tile
       SB x5, 0(x3)
       SB x5, 1(x3)
       SB x5, 2(x3)
       SB x5, 3(x3)
       SB x5, 4(x3)
       SB x5, 5(x3)
       SB x5, 6(x3)
       SB x5, 7(x3)

       # Move to next row
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
    LI x0, 10         # Ball X position (center)
    LI x1, 7          # Ball Y position (center)
    LI x2, 4          # Right paddle Y position

    # === Clear screen for game ===
    LUI x3, 480       # Reset to tile map base
    LI x4, 0          # Black tile

    # Clear entire screen area efficiently using address advancement
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    # Advance address by 8 and continue
    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    # Advance again
    ADDI x3, 4
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)

    # === Draw initial game field ===
    LI x4, 1          # White tile

    # Draw left paddle at column 0, rows 4-7
    LUI x3, 480
    ADDI x3, 63       # Add 63
    ADDI x3, 17       # Add 17 (total 80 = row 4)
    SB x4, 0(x3)
    ADDI x3, 20       # Row 5
    SB x4, 0(x3)
    ADDI x3, 20       # Row 6
    SB x4, 0(x3)
    ADDI x3, 20       # Row 7
    SB x4, 0(x3)

    # Draw right paddle at column 19
    LUI x3, 480
    MV x5, x2         # Get paddle Y position
    SLLI x5, 4        # y * 16
    ADD x3, x5
    MV x5, x2
    SLLI x5, 2        # y * 4
    ADD x3, x5        # x3 = base + y*20
    ADDI x3, 19       # Column 19

    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)
    ADDI x3, 20
    SB x4, 0(x3)

# === LINEAR PING PONG SIMULATION ===

    # === Frame 1: Ball starts at center ===
    LI x0, 10         # Ball X
    LI x1, 7          # Ball Y

    # Draw ball
    LUI x3, 480
    MV x5, x1
    SLLI x5, 4
    ADD x3, x5
    MV x5, x1
    SLLI x5, 2
    ADD x3, x5
    ADD x3, x0
    LI x4, 1
    SB x4, 0(x3)

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0
    SB x4, 0(x3)

    # === Check for input - simplified ===
    ECALL 7           # Read keyboard - key in x6 (a0), pressed flag in x7 (a1)

    # Process input without long branches
    LI x5, 111        # 'o' key
    SUB x5, x6        # Compare with pressed key
    BZ x5, move_right_up_simple  # If equal, it's nearby

    LI x5, 108        # 'l' key
    SUB x5, x6
    BZ x5, move_right_down_simple

    # Continue with game if no relevant input
    NOP
    NOP
move_right_up_simple:
    # Simple paddle movement - just adjust position
    BZ x2, skip_up    # Can't move up from position 0
    ADDI x2, -1       # Move up (decrease Y coordinate)
skip_up:
    NOP
    NOP

move_right_down_simple:
    # Simple paddle movement down
    LI x5, 10
    SLT x6, x2, x5    # Check if can move down
    BZ x6, skip_down
    ADDI x2, 1        # Move down (increase Y coordinate)
skip_down:
    NOP
    NOP

    # === Frame 2: Ball moves right-down ===
    ADDI x0, 1        # X = 11
    ADDI x1, 1        # Y = 8

    # Draw ball
    LUI x3, 480
    MV x5, x1
    SLLI x5, 4
    ADD x3, x5
    MV x5, x1
    SLLI x5, 2
    ADD x3, x5
    ADD x3, x0
    LI x4, 1
    SB x4, 0(x3)

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 3: Ball continues ===
    ADDI x0, 1        # X = 12
    ADDI x1, 1        # Y = 9

    # Draw ball
    LUI x3, 480
    MV x5, x1
    SLLI x5, 4
    ADD x3, x5
    MV x5, x1
    SLLI x5, 2
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Check for input again
       ECALL 7
       LI x5, 111
       SUB x5, x6
       BZ x5, input_up_2

       LI x5, 108
       SUB x5, x6
       BZ x5, input_down_2

       # Continue normally
       NOP
       NOP
       J continue_frame_3

   input_up_2:
       BZ x2, continue_frame_3
       ADDI x2, -1       # Move up (decrease Y)
       J continue_frame_3

   input_down_2:
       LI x5, 10
       SLT x6, x2, x5
       BZ x6, continue_frame_3
       ADDI x2, 1        # Move down (increase Y)

   continue_frame_3:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 4: Ball approaches bottom ===
       ADDI x0, 1        # X = 13
       ADDI x1, 1        # Y = 10

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Check for bottom wall collision
       LI x5, 13
       SLT x6, x1, x5
       BNZ x6, no_bottom_hit

       # Hit bottom - show indicator
       LUI x5, 480
       ADDI x5, 39
       LI x6, 1
       SB x6, 0(x5)

   no_bottom_hit:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 5: Ball bounces up from bottom ===
       ADDI x0, 1        # X = 14
       LI x1, 12         # Y = 12 (bounced up)

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Input check
       ECALL 7
       LI x5, 111
       SUB x5, x6
       BZ x5, input_up_3

       LI x5, 108
       SUB x5, x6
       BZ x5, input_down_3

       J continue_frame_5

input_up_3:
    BZ x2, continue_frame_5
    ADDI x2, -1       # Move up (decrease Y)
    J continue_frame_5

input_down_3:
    LI x5, 10
    SLT x6, x2, x5
    BZ x6, continue_frame_5
    ADDI x2, 1        # Move down (increase Y)

   continue_frame_5:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 6: Ball continues up-right ===
       ADDI x0, 1        # X = 15
       ADDI x1, -1       # Y = 11

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 7: Ball continues ===
       ADDI x0, 1        # X = 16
       ADDI x1, -1       # Y = 10

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Input check
       ECALL 7
       LI x5, 111
       SUB x5, x6
       BZ x5, input_up_4

       LI x5, 108
       SUB x5, x6
       BZ x5, input_down_4

       J continue_frame_7

   input_up_4:
       BZ x2, continue_frame_7
       ADDI x2, -1       # Move up (decrease Y)
       J continue_frame_7

   input_down_4:
       LI x5, 10
       SLT x6, x2, x5
       BZ x6, continue_frame_7
       ADDI x2, 1        # Move down (increase Y)

   continue_frame_7:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 8: Ball approaches right paddle ===
       ADDI x0, 1        # X = 17
       ADDI x1, -1       # Y = 9

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 9: Ball at right edge - check paddle collision ===
       ADDI x0, 1        # X = 18
       ADDI x1, -1       # Y = 8

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Check collision with right paddle
       SLT x5, x1, x2    # ball_y < paddle_top?
       BNZ x5, paddle_miss_right

       MV x5, x2
       ADDI x5, 4        # paddle_bottom = paddle_top + 4
       SLT x6, x1, x5    # ball_y < paddle_bottom?
       BZ x6, paddle_miss_right

       # Paddle hit! Show indicator
       LUI x5, 480
       ADDI x5, 18       # Top right indicator
       LI x6, 1
       SB x6, 0(x5)

       J continue_frame_9

   paddle_miss_right:
       # Ball missed - left player scores
       LUI x5, 480
       ADDI x5, 2        # Left score indicator
       LI x6, 1
       SB x6, 0(x5)

   continue_frame_9:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 10: Ball bounces left after paddle hit ===
       LI x0, 16         # X = 16 (moving left now)
       ADDI x1, -1       # Y = 7

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Input check
       ECALL 7
       LI x5, 111
       SUB x5, x6
       BZ x5, input_up_5

       LI x5, 108
       SUB x5, x6
       BZ x5, input_down_5

       J continue_frame_10

   input_up_5:
       BZ x2, continue_frame_10
       ADDI x2, -1       # Move up (decrease Y)
       J continue_frame_10

   input_down_5:
       LI x5, 10
       SLT x6, x2, x5
       BZ x6, continue_frame_10
       ADDI x2, 1        # Move down (increase Y)

   continue_frame_10:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 11: Ball continues left ===
       ADDI x0, -1       # X = 15
       ADDI x1, -1       # Y = 6

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 12: Ball hits top wall ===
       ADDI x0, -1       # X = 14
       LI x1, 1          # Y = 1 (at top, bounced)

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Show top wall hit
       LUI x5, 480
       ADDI x5, 1
       LI x6, 1
       SB x6, 0(x5)

       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 13: Ball moves left-down after top bounce ===
       ADDI x0, -1       # X = 13
       ADDI x1, 1        # Y = 2

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 14: Ball approaches left paddle ===
       LI x0, 2          # X = 2 (near left paddle)
       LI x1, 5          # Y = 5

       # Draw ball
       LUI x3, 480
       MV x5, x1
       SLLI x5, 4
       ADD x3, x5
       MV x5, x1
       SLLI x5, 2
       ADD x3, x5
       ADD x3, x0
       LI x4, 1
       SB x4, 0(x3)

       # Check left paddle collision (fixed at rows 4-7)
       LI x5, 4
       SLT x6, x1, x5
       BNZ x6, left_miss

       LI x5, 8
       SLT x6, x1, x5
       BZ x6, left_miss

       # Left paddle hit!
       LUI x5, 480
       LI x6, 1
       SB x6, 0(x5)      # Top left indicator

       J continue_final

   left_miss:
       # Right player scores
       LUI x5, 480
       ADDI x5, 19
       LI x6, 1
       SB x6, 0(x5)

   continue_final:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Clear ball
       LI x4, 0
       SB x4, 0(x3)

       # === Frame 15: Show final game state ===
       # Update right paddle visual based on final position
       LUI x3, 480
       MV x5, x2         # Get final paddle position
       SLLI x5, 4        # y * 16
       ADD x3, x5
       MV x5, x2
       SLLI x5, 2        # y * 4
       ADD x3, x5        # x3 = base + y*20
       ADDI x3, 19       # Column 19

       LI x4, 1          # Redraw paddle at final position
       SB x4, 0(x3)
       ADDI x3, 20
       SB x4, 0(x3)
       ADDI x3, 20
       SB x4, 0(x3)
       ADDI x3, 20
       SB x4, 0(x3)

       # === Final frame: Show game over pattern ===
          # === Create BIG, SOLID "GAME OVER" text pattern ===

          # Letter "G" - Position: Row 7, Col 1
          LUI x3, 480       # Reset to tile map
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 14       # 140 (row 7)
          ADDI x3, 1        # Column 1

          # G - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          # Row 3: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 4: ███
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █

          # Letter "A" - Position: Row 7, Col 5
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 14       # 140 (row 7)
          ADDI x3, 5        # Column 5

          # A - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 3: ███
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 4: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █

          # Letter "M" - Position: Row 7, Col 9
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 14       # 140 (row 7)
          ADDI x3, 9        # Column 9

          # M - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 3: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 4: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █

          # Letter "E" - Position: Row 7, Col 13
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 14       # 140 (row 7)
          ADDI x3, 13       # Column 13

          # E - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          # Row 3: ██
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          # Row 4: ███
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █

          # === "OVER" text below ===

          # Letter "O" - Position: Row 12, Col 3
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 63       # 189
          ADDI x3, 63       # 252 (row 12)
          ADDI x3, 3        # Column 3

          # O - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 3: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 4: ███
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █

          # Letter "V" - Position: Row 12, Col 7
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 63       # 189
          ADDI x3, 63       # 252 (row 12)
          ADDI x3, 7        # Column 7

          # V - make it 3x4 blocks
          # Row 1: █ █
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 3: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 4:  █
          ADDI x3, 20
          SB x4, 1(x3)      # █

          # Letter "E" - Position: Row 12, Col 11
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 63       # 189
          ADDI x3, 63       # 252 (row 12)
          ADDI x3, 11       # Column 11

          # E - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          # Row 3: ██
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          # Row 4: ███
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █

          # Letter "R" - Position: Row 12, Col 15
          LUI x3, 480       # Reset
          ADDI x3, 63       # 63
          ADDI x3, 63       # 126
          ADDI x3, 63       # 189
          ADDI x3, 63       # 252 (row 12)
          ADDI x3, 15       # Column 15

          # R - make it 3x4 blocks
          # Row 1: ███
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          SB x4, 2(x3)      # █
          # Row 2: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █
          # Row 3: ██
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 1(x3)      # █
          # Row 4: █ █
          ADDI x3, 20
          SB x4, 0(x3)      # █
          SB x4, 2(x3)      # █

       # Final delay before program end
       NOP
       NOP
       NOP
       NOP
       NOP
       NOP
       NOP
       NOP

       ECALL 10          # Exit program