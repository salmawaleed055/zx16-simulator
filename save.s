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
    # x0 = ball X position
    # x1 = ball Y position
    # x7 = collision flag (0 = no collision, 1 = paddle hit, 2 = frame hit)

    LI x0, 10         # Ball starts at center X
    LI x1, 6          # Ball starts at middle Y
    LI x7, 0          # No collision initially
   # === Create BIG, SOLID "PONG GAME" text pattern ===

    # Letter "P" - Position: Row 2, Col 1 (make it bigger and more solid)
    LUI x3, 480       # Reset to tile map
    ADDI x3, 40       # Row 2: 2*20 = 40
    ADDI x3, 1        # Column 1

    # P - make it 3x4 blocks for visibility
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
    # Row 4: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
 # Row 5: █
    ADDI x3, 20
    SB x4, 0(x3)      # █

    # Letter "O" - Position: Row 2, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 5        # Column 5

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
   # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

 # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "N" - Position: Row 2, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 9        # Column 9

    # N - make it 3x4 blocks
    # Row 1: █ █
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    # Row 2: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
 SB x4, 1(x3)      # █
 SB x4, 3(x3)      # █

    # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)
SB x4, 3(x3)

     # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
   # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █

       # Letter "G" - Position: Row 2, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 14      # Column 13

  # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
     # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
        # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
  # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █



    # === "GAME" text below ===

    # Letter "G" for GAME - Position: Row 8, Col 1
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 35       # 160 (row 8)
    ADDI x3, 0        # Column 1

   # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
     # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
        # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
  # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █



    # Letter "A" - Position: Row 8, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 5        # Column 5

    # A - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
      ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "M" - Position: Row 8, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 9        # Column 9

    # M - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "E" - Position: Row 8, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 13       # Column 13

    # E - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)
  ADDI x3, 20
    SB x4, 0(x3)      # █

    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █


 LUI x3, 480       # Reset to tile map
    LI x4, 0          # Black tile

    ADDI x3, 40       # Row 2: 2*20 = 40
    ADDI x3, 1        # Column 1

    LUI x3, 480       # Reset to tile map
    ADDI x3, 40       # Row 2: 2*20 = 40
    ADDI x3, 1        # Column 1

    # P - make it 3x4 blocks for visibility
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
    # Row 4: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
 # Row 5: █
    ADDI x3, 20
    SB x4, 0(x3)      # █

    # Letter "O" - Position: Row 2, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 5        # Column 5

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
   # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

 # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "N" - Position: Row 2, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 9        # Column 9

    # N - make it 3x4 blocks
    # Row 1: █ █
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    # Row 2: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
 SB x4, 1(x3)      # █
 SB x4, 3(x3)      # █

    # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)
SB x4, 3(x3)

     # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
   # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █

       # Letter "G" - Position: Row 2, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 14      # Column 13

  # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
     # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
        # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
  # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █



    # === "GAME" text below ===

    # Letter "G" for GAME - Position: Row 8, Col 1
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 35       # 160 (row 8)
    ADDI x3, 0        # Column 1

   # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
     # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
        # Row 1: ███
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
  # Row 4: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █



    # Letter "A" - Position: Row 8, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 5        # Column 5

    # A - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
      ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "M" - Position: Row 8, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 9        # Column 9

    # M - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
 ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "E" - Position: Row 8, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 13       # Column 13

    # E - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)
  ADDI x3, 20
    SB x4, 0(x3)      # █

    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █


# === NOW SHOW PADDLES ON CLEAR SCREEN ===

    LI x4, 1          # White tile

    # LEFT PADDLE - Column 0, Rows 3-8 (6 tiles tall)
    LUI x3, 480       # Reset to tile map
    ADDI x3, 60       # Row 3: 3*20 = 60
    ADDI x3, 0        # Column 0 (leftmost)

    # Draw left paddle (6 tiles vertical)
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

    # RIGHT PADDLE - Column 19, Rows 3-8 (6 tiles tall)
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


    LI x4, 0          # Black tile

game_input_loop:
 li x6, 440      # Frequency for sound
   li x7, 100      # Duration
   ecall 4         # SYS_TONE
    # === Check for keyboard input ===
    ECALL 7           # Read keyboard - key in x6 (a0), pressed flag in x7 (a1)

    # Check if a key was pressed
    # CHANGE THIS:
    # BZ x7, continue_game    # This branch is too far
    # TO THIS:
    BNZ x7, check_keys      # Short branch to nearby code
    J continue_game         # Jump to distant target

check_keys:
    # Check for right paddle controls
    LI x5, 111        # ASCII code for 'o' (move up)
    # CHANGE THIS:
    # BEQ x6, x5, move_right_paddle_up    # This branch is too far
    # TO THIS:
    BEQ x6, x5, do_move_up              # Short branch to nearby jump

    LI x5, 108        # ASCII code for 'l' (move down)
    # CHANGE THIS:
    # BEQ x6, x5, move_right_paddle_down  # This branch is too far
    # TO THIS:
    BEQ x6, x5, do_move_down            # Short branch to nearby jump

    J continue_game   # If not our keys, continue

do_move_up:
    J move_right_paddle_up    # Jump to the actual movement code

do_move_down:
    J move_right_paddle_down  # Jump to the actual movement code

move_right_paddle_up:
    # Check if paddle can move up (top row > 0)
    # CHANGE THIS:
    # BZ x2, continue_game   # This branch is too far
    # TO THIS:
    BNZ x2, can_move_up     # Short branch
    J continue_game         # Jump to distant target

can_move_up:
    # [Rest of your paddle movement code stays the same]

    J continue_game    # Jump back instead of falling through

move_right_paddle_down:
    # Check if paddle can move down (bottom row < 9)
    # Paddle is 6 tiles tall, so bottom row = top row + 5
    MV x5, x2         # Copy paddle position
    ADDI x5, 5        # Calculate bottom row
    LI x6, 9          # Maximum bottom row
    # CHANGE THIS:
    # BGE x5, x6, continue_game  # This branch is too far
    # TO THIS:
    BLT x5, x6, can_move_down   # Short branch (opposite condition)
    J continue_game             # Jump to distant target

can_move_down:
    # [Rest of your paddle movement code stays the same]

    J continue_game    # Jump back instead of falling through


    # Clear old paddle position
    LUI x3, 480       # Reset to tile map
    MV x5, x2         # Get current paddle Y position
    SLLI x5, 4        # y * 16
    MV x6, x2         # Copy Y again
    SLLI x6, 2        # y * 4
    ADD x5, x6        # y * 20
    ADD x3, x5        # Add to base address
    ADDI x3, 19       # Column 19 (rightmost)

    LI x4, 0          # Black tile to clear

    # Clear 6 tiles vertically at old position
    SB x4, 0(x3)      # Row 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 1
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 2
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5

    # Move paddle up
    ADDI x2, -1

    # Draw new paddle position
    LUI x3, 480       # Reset to tile map
    MV x5, x2         # Get new paddle Y position
    SLLI x5, 4        # y * 16
    MV x6, x2         # Copy Y again
    SLLI x6, 2        # y * 4
    ADD x5, x6        # y * 20
    ADD x3, x5        # Add to base address
    ADDI x3, 19       # Column 19 (rightmost)

    LI x4, 1          # White tile

    # Draw 6 tiles vertically at new position
    SB x4, 0(x3)      # Row 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 1
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 2
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5

    J continue_game


    # Clear old paddle position
    LUI x3, 480       # Reset to tile map
    MV x5, x2         # Get current paddle Y position
    SLLI x5, 4        # y * 16
    MV x6, x2         # Copy Y again
    SLLI x6, 2        # y * 4
    ADD x5, x6        # y * 20
    ADD x3, x5        # Add to base address
    ADDI x3, 19       # Column 19 (rightmost)

    LI x4, 0          # Black tile to clear

    # Clear 6 tiles vertically at old position
    SB x4, 0(x3)      # Row 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 1
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 2
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5

    # Move paddle down
    ADDI x2, 1

    # Draw new paddle position
    LUI x3, 480       # Reset to tile map
    MV x5, x2         # Get new paddle Y position
    SLLI x5, 4        # y * 16
    MV x6, x2         # Copy Y again
    SLLI x6, 2        # y * 4
    ADD x5, x6        # y * 20
    ADD x3, x5        # Add to base address
    ADDI x3, 19       # Column 19 (rightmost)

    LI x4, 1          # White tile

    # Draw 6 tiles vertically at new position
    SB x4, 0(x3)      # Row 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 1
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 2
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 3
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5

continue_game:
    # === Frame 1: Ball at (10, 6) ===
    # Calculate ball position: 0xF000 + y*20 + x
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

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

       # === Frame 2: Ball moves to (1, 9) - Near left edge, MISS PADDLE ===
       LI x0, 1          # Move ball to near left edge
       LI x1, 9          # Y position BELOW paddle range

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

       # Check collision with left side
       LI x5, 3          # Paddle starts at row 3
       SLT x6, x1, x5    # Check if ball Y < paddle start
       BNZ x6, left_miss # Ball is above paddle

       LI x5, 9          # Paddle ends after row 8
       SLT x6, x1, x5    # Check if ball Y < paddle end
       BZ x6, left_miss  # Ball is below paddle

       # Ball Y is within paddle range - it bounces
       LI x7, 1          # Set paddle collision flag
       J continue_frame2

   left_miss:
       # Ball missed paddle - RIGHT PLAYER SCORES!
       LI x7, 2          # Set frame collision flag

       # Clear the ball immediately (it disappears)
       LI x4, 0          # Black tile
       SB x4, 0(x3)      # Clear ball

       # Update score - Right player gets a point
       # Show score indicator at top right (position 15,0)
       LUI x3, 480       # 0xF000
       ADDI x3, 15       # Position for right score
       LI x4, 1          # White tile as score marker
       SB x4, 0(x3)      # Show right player scored

   continue_frame2:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Only clear ball if it hit paddle (not already cleared)
       LI x5, 1
       BNE x7, x5, skip_clear2
       LI x4, 0          # Black tile
       SB x4, 0(x3)      # Clear ball
   skip_clear2:
frame_hit1:
    # Ball missed paddle - it's a frame hit
    LI x7, 2          # Set frame collision flag

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

    # === Frame 3: Ball resets to center after frame hit ===
    LI x0, 10         # Reset to center X
    LI x1, 6          # Reset to center Y

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
    SB x4, 0(x3)      # Draw ball at center

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

    # === Frame 4: Ball moves to (1, 5) - Near left paddle, HIT ===
    LI x0, 1          # Move ball to near left edge
    LI x1, 5          # Y position WITHIN paddle range

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

    # Check collision with left paddle
    LI x5, 3          # Paddle starts at row 3
    SLT x6, x1, x5    # Check if ball Y < paddle start
    BNZ x6, no_paddle_hit1

    LI x5, 9          # Paddle ends after row 8
    SLT x6, x1, x5    # Check if ball Y < paddle end
    BZ x6, no_paddle_hit1

    # Ball Y is within paddle range - paddle hit!
    LI x7, 1          # Set paddle collision flag

no_paddle_hit1:
    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

      # === Frame 5: Ball moves to (18, 2) - Near right edge, ABOVE PADDLE ===
          LI x0, 18         # Move ball to near right edge
          LI x1, 2          # Y position ABOVE paddle range

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

          # Check collision with right side - NOW USING DYNAMIC PADDLE POSITION
          MV x5, x2         # Use dynamic paddle start position (stored in x2)
          SLT x6, x1, x5    # Check if ball Y < paddle start
          BNZ x6, right_miss # Ball is above paddle

          MV x5, x2         # Get paddle start again
          ADDI x5, 6        # Paddle ends after 6 tiles
          SLT x6, x1, x5    # Check if ball Y < paddle end
          BZ x6, right_miss  # Ball is below paddle

          # Ball Y is within paddle range - it bounces
          LI x7, 1          # Set paddle collision flag
          J continue_frame5

   right_miss:
       # Ball missed paddle - LEFT PLAYER SCORES!
       LI x7, 2          # Set frame collision flag

       # Clear the ball immediately (it disappears)
       LI x4, 0          # Black tile
       SB x4, 0(x3)      # Clear ball

       # Update score - Left player gets a point
       # Show score indicator at top left (position 4,0)
       LUI x3, 480       # 0xF000
       ADDI x3, 4        # Position for left score
       LI x4, 1          # White tile as score marker
       SB x4, 0(x3)      # Show left player scored

   continue_frame5:
       # Delay
       NOP
       NOP
       NOP
       NOP

       # Only clear ball if it hit paddle
       LI x5, 1
       BNE x7, x5, skip_clear5
       LI x4, 0          # Black tile
       SB x4, 0(x3)      # Clear ball
   skip_clear5:
    # === Frame 7: Diagonal movement (right-down) ===
    LI x0, 12         # Move right
    LI x1, 8          # Move down

    # Calculate position and draw
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

    # Clear
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 8: Hit bottom wall, bounce up ===
    LI x0, 14         # Continue right
    LI x1, 10         # Near bottom

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

    # Check if hit bottom (Y >= 14)
    LI x5, 14
    SLT x6, x1, x5
    BNZ x6, no_bottom_hit

    # Hit bottom - show bounce indicator
    LUI x3, 480
    ADDI x3, 39       # Position (19,1) for wall hit
    LI x4, 1
    SB x4, 0(x3)

no_bottom_hit:
    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LUI x3, 480
    MV x5, x1
    SLLI x5, 4
    ADD x3, x5
    MV x5, x1
    SLLI x5, 2
    ADD x3, x5
    ADD x3, x0
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 9: Bouncing up-right after bottom hit ===
    LI x0, 16         # Continue right
    LI x1, 8          # Moving up now

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

    # Clear
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 10: Hit right paddle at angle ===
    LI x0, 18         # At right edge
    LI x1, 6          # Mid paddle height

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

    # Clear
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 11: Bounce back left-up ===
    LI x0, 16         # Moving left
    LI x1, 4          # Moving up

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

    # Clear
    LI x4, 0
    SB x4, 0(x3)

    # === Frame 12: Hit top wall ===
    LI x0, 14         # Continue left
    LI x1, 0          # At top

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

    # Show top wall hit indicator
    LUI x3, 480
    ADDI x3, 38       # Position (18,1)
    LI x4, 1
    SB x4, 0(x3)

    # Delay
    NOP
    NOP
    NOP
    NOP

frame_hit2:
    # Ball missed paddle - it's a frame hit
    LI x7, 2          # Set frame collision flag

    # Delay
    NOP
    NOP
    NOP
    NOP

    # Clear ball
    LI x4, 0          # Black tile
    SB x4, 0(x3)      # Clear ball

    # === Frame 6: Ball resets to center again ===
    LI x0, 10         # Reset to center X
    LI x1, 6          # Reset to center Y

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
    SB x4, 0(x3)      # Draw ball at center

    # Delay
    NOP
    NOP
    NOP
    NOP

    # === Show collision status ===
    # x7 = 1: paddle hit (show at 0,0)
    # x7 = 2: frame hit (show at 1,0)
    ADDI x7, -1       # Subtract 1 to check value
    BZ x7, show_paddle_hit

    # Frame hit indicator at (1,0)
    LUI x3, 480       # 0xF000
    ADDI x3, 1        # Position (1,0)
    LI x4, 1          # White tile
    SB x4, 0(x3)      # Draw frame hit indicator
    NOP
    NOP

show_paddle_hit:
    BNZ x7, end_demo  # Skip if not paddle hit

    # Paddle hit indicator at (0,0)
    LUI x3, 480       # 0xF000
    LI x4, 1          # White tile
    SB x4, 0(x3)      # Draw paddle hit indicator

end_demo:
     # Show final score summary
     # Left player score indicator
     LUI x3, 480       # 0xF000
     ADDI x3, 4        # Left score position
     LB x5, 0(x3)      # Check if left scored

     # Right player score indicator
     ADDI x3, 11       # Move to right score position (15-4=11)
     LB x6, 0(x3)      # Check if right scored

     # Display winner at center
     LUI x3, 480
     ADDI x3, 40
     ADDI x3, 40
     ADDI x3, 30       # Position (10,5)

     # If left player scored (has indicator), show 'L'
     BZ x5, check_right_win
     LI x4, 1
     SB x4, 0(x3)      # Show left won
     J finish

 check_right_win:
     # If right player scored, show 'R' as a 3x4 block
     j finish

     # Set base address for 'R' at center of screen
     LI16  x3, 0xF000        # base address for tilemap
     ADDI  x3, x3, 40         # row 5 (row index 5 * 20 = 100)
     ADDI  x3, x3, 40         # column 15 (column index 15 * 1 = 15)
     # Now x3 points to row 5, col 15

     LI    x4, 1              # block pixel value

     # Draw top row: "███"
     SB    x4, 0(x3)          # row 1, col 15
     SB    x4, 1(x3)
     SB    x4, 2(x3)

     # Move to next row: row 2, col 14-16
     ADDI  x3, x3, 20        # next row
     ADDI  x3, x3, -1        # move to col 14
     SB    x4, 0(x3)
     SB    x4, 2(x3)          # skip col 15 for spacing

     # Next row: row 3, col 15-17
     ADDI  x3, x3, 1          # move to col 15
     ADDI  x3, x3, 20         # next row
     SB    x4, 0(x3)
     SB    x4, 1(x3)
     SB    x4, 2(x3)

     # Next row: row 4, col 15
     ADDI  x3, x3, 20        # next row
     SB    x4, 0(x3)

     # Next row: row 5, col 15
     ADDI  x3, x3, 20        # next row
     SB    x4, 0(x3)


 finish:
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

     NOP
     NOP
     NOP
     NOP
     ECALL 10