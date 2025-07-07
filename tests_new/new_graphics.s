.org 0x0000
.text

main:
    lui t0, 0x1E4 
    sb t1, 0(t0)
    li t1, 0x03
    li t1, 0x03
    sb t1, 0(t0)

    ecall 0x3FF 

