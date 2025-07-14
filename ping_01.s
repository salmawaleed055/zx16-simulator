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

    # === OPTIONAL: Add center dashed line ===
    LUI x3, 480       # Reset to tile map
    ADDI x3, 10       # Column 10 (center)

    # Draw dashed line down the center (every other row)
    SB x4, 0(x3)      # Row 0
    ADDI x3, 40       # Skip 2 rows (2*20 = 40)
    SB x4, 0(x3)      # Row 2
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 4
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 6
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 8


    LI x4, 0          # Black tile

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

    # Clear center dashed line
    LUI x3, 480       # Reset to tile map
    ADDI x3, 10       # Column 10 (center)

    # Clear dashed line down the center
    SB x4, 0(x3)      # Row 0
    ADDI x3, 40       # Skip 2 rows (2*20 = 40)
    SB x4, 0(x3)      # Row 2
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 4
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 6
    ADDI x3, 40       # Skip 2 rows
    SB x4, 0(x3)      # Row 8

    # === LOOP BACK TO START ===


      ECALL 10