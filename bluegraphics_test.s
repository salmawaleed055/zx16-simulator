.org 0x0020
.text

main:
    # lui s0, 0x092 # load palette base address
    # ori s0, 0x2C
    # lui s0, 18
    # ori s0, 0x2Cs
    li s0, 0xFA00

    li s1, 0x03 # load blue into s1
    sb s1, 4(s0) # store blue in palette

    # lui t1, 2  # load tile data base address
    # ori t1, 0x2C
    li t1, 0xF200

    li s1, 0x44 # tile 0
    li s0, 63
    addi s0, 63
    addi s0, 2
    li t0, 0 # counter
    add x0, x0
    loop:
        bge  t0, s0, done
        sb s1, 0(t1)
        addi t1, 1
        addi t0, 1
        j loop

    done:
        li t1, 0xF000 # load tile map base address
        li s1, 0x01
        # lui s0, 0
        # ori s0, 0x12C
        li s0, 300
        li t0, 0 # counter
        add x0, x0
        loop2:
            bge t0, s0, exit
            sb s1, 0(t1)
            addi t0, 1
            addi t1, 1
            j loop2

        
        exit:
            ecall 0x3FF

    
