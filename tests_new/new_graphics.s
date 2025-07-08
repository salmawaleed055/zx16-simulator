.org 0x0000
.text

main:
    li t0, 0 
    li t1, 0x03
    sb t1, 0(t0)
    li t0, 0x012C
    sb t1, 0(t0)
    li t0, 0xFA00
    sb t1, 0(t0)
    ecall 0x3FF 

