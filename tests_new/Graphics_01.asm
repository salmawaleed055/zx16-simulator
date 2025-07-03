li x1, 5
li x2, 3
add x1, x2
ecall 0x3FF

li x1, 0xF000      ; Tile map base
li x2, 1           ; Tile index
sw x1, x2, 0       ; tileMap[0] = 1

li x1, 0xF200      ; Tile data base
li x2, 0x21        ; tile pattern
sw x1, x2, 0       ; tileData[0] = 0x21

li x1, 0xFA00      ; Color palette base
li x2, 0xFF        ; white
sw x1, x2, 0       ; palette[0] = white

ecall 0
