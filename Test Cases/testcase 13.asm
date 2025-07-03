.data
.org 0x100
g: .asciiz "branch_taken"

.text
.org 0
main:
    li a0, -5
    li t0, 3
    blt a0, t0, less_than_label
    li a0, 1     # should not be executed
    j exit         # Jump to exit

less_than_label:
ecall 1
    lui a0, %hi(g)
    addi a0, %lo(g)
    ecall 5

exit:
    ecall 3        # Exit the program
