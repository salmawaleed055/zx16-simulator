.org 0x0000
.text

main:
loop:
    ecall 7          # read keyboard, returns key in a0, pressed flag in a1
    li t0, 1
    beq a1, t0, key_pressed   # if a1 == 1, key pressed
    j loop                   # else loop again

key_pressed:
    mv a1, a0               # move key code to a1 for printing
                     # print character in a1 (adjust if your print syscall is different)

    li t1, 113              # ASCII for 'q'
    beq a0, t1, exit        # if key is 'q', exit

    j loop

exit:
    ecall 0x3FF             # exit program
