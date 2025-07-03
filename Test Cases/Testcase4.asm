.text
.org 0

main:
    lui a0, %hi(w)
    addi a0, a0, %lo(w)
    ecall 1
    ecall 5

    li t0, 12
    sw t0, 0(a0)
    lw t1, 0(a0)

    mv a0, t1
    ecall 1

    lui a0, %hi(b)
    addi a0, a0, %lo(b)
    ecall 1

    lbu t0, 0(a0)
    lb  t1, 0(a0)

    mv a0, t0
    ecall 1

    mv a0, t1
    ecall 1

    li t0, 0xC3
    sb t0, 0(a0)

    lbu t1, 0(a0)
    mv a0, t1
    ecall 1

    li s0, 5
    li s1, 3

    beq s0, s1, equal_case
    bne s0, s1, not_equal_case

    li a0, 0
    ecall 1
    j end_program

equal_case:
    li a0, 1
    ecall 1
    j end_program

not_equal_case:
    li a0, 2
    ecall 1
    j end_program

    li s0, 10
    li s1, 5
    blt s0, s1, less_than_case
    bge s0, s1, greater_or_equal_case

less_than_case:
    li a0, 3
    ecall 1
    j end_program

greater_or_equal_case:
    li a0, 4
    ecall 1
    j end_program

end_program:
    ecall 3

.data
.org 0x100
    w: .asciiz "test"
    b: .byte 0x8A
