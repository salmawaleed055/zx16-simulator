
.org 0x0000

.text
start:
    li a0, 0
    ecall 1
    addi a0, 1
    ecall 1
    addi a0, -6
    ecall 1
    li a1, 5
    mv a0, a1
    slli a0, 2
    ecall 1

    ecall 3
