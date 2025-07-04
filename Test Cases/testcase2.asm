# Edge Cases (Zero Registers, Negative Values)
.org 0x0000

.text
start:
    li a1,5
    slti  a0, a1, -1
    xor a0,a0,0xF
    bz a0,label
    mv    a0, a1

    label:
    ecall 1
    ecall 3
