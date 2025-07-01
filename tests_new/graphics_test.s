# graphics_test.s - ZX16 Graphics System Test
.text
.org 0x0020

main:
    # Initialize stack pointer
    LI16 sp, 0xEFFE
    
    # 1. Set color palette (16 colors starting at 0xFA00)
    LI16 x1, 0xFA00      # Palette base address
    LI x2, 0x00          # Color 0: Black (RGB=0,0,0)
    SB x2, 0(x1)
    LI x2, 0xE0          # Color 1: Red (RGB=7,0,0)
    SB x2, 1(x1)
    LI x2, 0x1C          # Color 2: Green (RGB=0,7,0)
    SB x2, 2(x1)
    LI x2, 0x03          # Color 3: Blue (RGB=0,0,3)
    SB x2, 3(x1)
    
    # 2. Define Tile 1 (solid color 1)
    LI16 x1, 0xF280      # Tile 1 data starts at 0xF200 + 128
    LI x2, 0x11          # Two pixels of color 1 (0x11 = 00010001)
    LI x3, 128           # 128 bytes per tile
fill_tile:
    SB x2, 0(x1)         # Store byte
    ADDI x1, x1, 1       # Next address
    ADDI x3, x3, -1      # Decrement counter
    BNZ x3, fill_tile     # Loop until done
    
    # 3. Write to tile map (20x15 grid starting at 0xF000)
    LI16 x1, 0xF000      # Tile map base
    LI x2, 1             # Tile index 1
    LI x3, 0             # Column counter
    LI x4, 0             # Row counter

draw_columns:
    SB x2, 0(x1)         # Store tile index
    ADDI x1, x1, 1       # Next position
    ADDI x3, x3, 1       # Increment column
    BLTU x3, 20, draw_columns  # 20 columns per row
    
    # Next row
    LI x3, 0             # Reset column counter
    ADDI x4, x4, 1       # Next row
    BLTU x4, 15, draw_columns  # 15 rows
    
    # 4. Exit
    ECALL 10             # Program exit

.data
# No data needed
