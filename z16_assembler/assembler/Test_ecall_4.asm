    .org 0x0000
    .text

    main:
    li a0, 500
    li a1,500
    ecall 4

    li a0, 500
    li a1,200
    ecall 4

    li a0, 0
    li a1,900
    ecall 4

    ecall 0x3FF
