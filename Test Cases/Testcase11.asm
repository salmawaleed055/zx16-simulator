
.data
.org 0x100
l: .asciiz "less"
g: .asciiz "branch_not_taken"
u: .asciiz "less_unsigned"
gt: .asciiz "greater_than"

.text
.org 0
start:
    li   a0, 8
    li   a1, 9
    li   t1, 3

    blt  a0, a1, less_than

    lui  a0, %hi(g)
    addi a0, %lo(g)
    ecall 5
    j    test_bltu

less_than:
    lui  a0, %hi(l)
    addi a0, %lo(l)
    ecall 5  #less printed

test_bltu:
    li   a0, 8
    li   a1, 9
    bltu a0, a1, less_than_unsigned

    lui  a0, %hi(g) #not executed
    addi a0, %lo(g)
    ecall 5
    j    test_bgeu

less_than_unsigned:
    lui  a0, %hi(u)
    addi a0, %lo(u)
    ecall 5 #less unsugned printed

test_bgeu:
    li   t1, 3
    li   a0, 8
    bgeu t1, a0, greater_than

    lui  a0, %hi(g) #branch is not taken printed
    addi a0, %lo(g)
    ecall 5
    j    end_program #exit the program after jumping to end

greater_than:
    lui  a0, %hi(gt)
    addi a0, %lo(gt)
    ecall 5

end_program:
    ecall 3
