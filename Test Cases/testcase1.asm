    li a1,5
    li t0,8
   andi a0,t0, 3
     beq a0,t0, label
      mv    a0, a1

      label:
      ecall 1
      ecall 3


