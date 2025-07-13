#arithmatic
    .text
    .org 0x0000

main:
    li x1, 7          # x1 = 7
    li x2, 3          # x2 = 3
    add x3, x2
    sub x4, x2
    mv  x5, x4
    clr x6
    addi x6, 1
    ecall 10          # Exit

