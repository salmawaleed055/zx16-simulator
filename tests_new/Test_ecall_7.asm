.org 0x0000
.text

main:
poll:
    ecall 7           # Read keyboard: a0 = key code, a1 = 1 if pressed, 0 if not
    bz   a1, poll     # If a1 == 0 (no key pressed), loop

    # Print the key code as a decimal
    ecall 4           # Print decimal value in a0

    ecall 10          # Exit

