# System Calls 

    .text
    .org    0
main:
li t0, 7
srai t0, 1
mv a0, t0
ecall 1
srli t0, 1
and a0, t0
ecall 1
auipc a0, 1
ecall 1
bne t0, a0, else
add a0, t0
ecall 1
add a0, t0
add a0, t0
add a0, t0
else:
ecall 1
 
ecall 3 