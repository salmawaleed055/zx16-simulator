    .text
    .org 0x0000

main:
    li x1, 5
    li x2, 5
    beq x1, x2, success   # Branch taken
    clr x6                # Fail: x6 = 0
    ecall 10

success:
    nop
    li x6, 1              # Pass: x6 = 1
    ecall 10
