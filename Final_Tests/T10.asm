# ECALL service numbers
.text
.org 0x000
    j   main

.org 0x0020
main:
    # Set initial volume to 200 (out of 255)
    li      x6, 200
    ecall   5               # SYS_VOL

    # C C G G A A G (Twinkle Twinkle opening)
    # Note: Frequencies in Hz, durations in ms

    # C (261 Hz)
    li      x6, 261
    li      x7, 400
    ecall   4               # SYS_TONE

    li      x6, 261
    li      x7, 400
    ecall   4

        li      x6, 261
        li      x7, 400
        ecall   4

    # G (392 Hz)
    li      x6, 392
    li      x7, 400
    ecall   4

    li      x6, 392
    li      x7, 400
    ecall   4


        li      x6, 392
        li      x7, 400
        ecall   4

    # A (440 Hz)
    li      x6, 440
    li      x7, 400
    ecall   4

    li      x6, 440
    li      x7, 400
    ecall   4



        li      x6, 440
        li      x7, 400
        ecall   4

    # G (392 Hz) - hold longer for phrase ending
    li      x6, 392
    li      x7, 800
    ecall   4

    # Optional: Stop audio to ensure clean end
    ecall   6               # SYS_AUDIO_STOP

    # Exit program
    ecall   10              # SYS_EXIT

.data
# (No data needed for this test)

