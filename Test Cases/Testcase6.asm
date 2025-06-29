# Jumping

    .text
    .org    0
main:

        li a0, 5
        sltui a0, 120
        li t0, 6
        bge a0, t0, else
        ecall 1
        j exit
        else:
        addi t0, 2
        jalr ra, t0
        exit:
        mv a0, ra
        ecall 1
        ecall 3


