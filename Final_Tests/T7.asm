# ZX16 Loop Test: Countdown from 5 to 0 - Fixed Version

.text
.org 0x0020
main:
    li x1, 5          # x1 = 5 (loop counter)
    li x2, 0          # x2 = 0 (accumulator/result)

loop_start:
    add x2, x1        # x2 += x1
    dec x1            # x1 -= 1
    bz x1, loop_end   # If x1 == 0, exit loop
    j loop_start      # Otherwise, continue loop

loop_end:
    li a0, 1          # Indicate success
    ecall 3           # Exit
