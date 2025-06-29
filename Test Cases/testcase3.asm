.org 0x0000
.text
start:
    li  a1,2
    li  t0,2
    add a0,a1,t0
    bnz   x0, label
    mv    a0, a1
label:

    ecall 1

    ecall 3