.data
.org 0x100
l: .asciiz "less"    
g: .asciiz "branch_not_taken" 

.text
.org 0
main:
    li a0, 5
    li t0, 10
    bltu a0, t0, unsigned_less_than_label
    li a0, -1 #not executed

unsigned_less_than_label:
    lui a0, %hi(l)
    addi a0, %lo(l)
    ecall 5
    li a1, 20
    bgeu t0, a1, greater_or_equal_label
    li t1, -2
     lui a0, %hi(g)
        addi a0, %lo(g)
        ecall 5


greater_or_equal_label:

    ecall 3
