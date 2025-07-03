.text
    .org 0

main:
    lui t0, %hi(data)
    addi t0, t0, %lo(data)
    li t1, 42
    sw t1, 0(t0)

    lw a0, 0(t0)
    ecall 1

     li t1, 0xF0
     sb t1, 2(t0)

    lb a0, 2(t0)
    ecall 1

    lbu a0, 2(t0)
    ecall 1

    li t1, 23
    sw t1, 4(t0)

    lw a0, 4(t0)
    ecall 1

    ecall 3

.data
    .org 0x100
data:
    .word 0, 0, 0
    .byte 0, 0, 0
