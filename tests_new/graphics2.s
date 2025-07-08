.org 0x0020
.text

main:
    # li s0, 0x092C # load palette base address
    lui s0, 18
    ori s0, 0x2C

    li s1, 0x03 # load blue into s1
    sb s1, 0(s0) # store blue in palette

    # li t1, 0x012C # load tile data base address
    lui t1, 2 
    ori t1, 0x2C

    li s1, 0x44 # tile 0
    # li s0, 127
    li s0, 63
    addi s0, 63
    addi s0, 1
    li t0, 0 # counter
    add x0, x0
    loop:
        bge  t0, s0, done
        sb s1, t0(t1)
        addi t0, 1
        j loop

    done:
        li t1, 0x0000 # load tile map base address
        li s1, 0
        # li s0, 299
        lui s0, 1
        ori s0, 43
        li t0, 0 # counter
        add x0, x0
        loop2:
            bge t0, s0, exit
            sb s1, t0(t1)
            addi t0, 1
            j loop2

        
        exit:
            ecall 0x3FF

    

