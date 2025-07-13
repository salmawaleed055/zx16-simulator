# ECALL service numbers
.equ    SYS_GETSTR,        0x001   # Read string
.equ    SYS_GETINT,        0x002   # Read integer
.equ    SYS_PUTSTR,        0x003   # Print string
.equ    SYS_TONE,          0x004   # Play tone
.equ    SYS_VOL,           0x005   # Set volume
.equ    SYS_AUDIO_STOP,    0x006   # Stop audio
.equ    SYS_GETKEY,        0x007   # Read keyboard
.equ    SYS_REGS_DUMP,     0x008   # Dump registers
.equ    SYS_MEM_DUMP,      0x009   # Dump memory
.equ    SYS_EXIT,          0x00A   # Exit program

.text
.org 0x000

main:
    # Test 1: Print String (Service 3)
    la      x6, welcome
    ecall   SYS_PUTSTR               # SYS_PUTSTR

    # Test 2: Read String (Service 1)
    la      x6, string_buffer
    li      x7, 50          # Buffer size
    ecall   SYS_GETSTR               # SYS_GETSTR

    # Test 3: Print the string we just read
    la      x6, string_buffer
    ecall   SYS_PUTSTR               # SYS_PUTSTR

    # Test 4: Read Integer (Service 2)
    la      x6, int_prompt
    ecall   SYS_PUTSTR               # SYS_PUTSTR
    ecall   SYS_GETINT               # SYS_GETINT
    mv      x1, x6          # Save integer in x1

    # Test 5: Set Audio Volume (Service 5)
    li      x6, 128         # Volume level
    ecall   SYS_VOL               # SYS_VOL

    # Test 6: Play Tone (Service 4)
    li      x6, 440         # Frequency (440 Hz)
    li      x7, 1000        # Duration (1000 ms)
    ecall   SYS_TONE               # SYS_TONE

    # Test 7: Stop Audio (Service 6)
    ecall   SYS_AUDIO_STOP               # SYS_AUDIO_STOP

    # Test 8: Read Keyboard (Service 7)
    ecall   SYS_GETKEY               # SYS_GETKEY
    mv      x2, x6          # Save key code in x2

    # Test 9: Memory Dump (Service 9)
    la      x6, welcome     # Start address
    li      x7, 32          # Number of bytes
    ecall   SYS_MEM_DUMP               # SYS_MEM_DUMP

    # Test 10: Registers Dump (Service 8)
    ecall   SYS_REGS_DUMP               # SYS_REGS_DUMP

    # Test 11: Exit Program (Service 10)
    la      x6, goodbye
    ecall   SYS_PUTSTR               # SYS_PUTSTR
    ecall   SYS_EXIT              # SYS_EXIT

.data
welcome:
    .string     "Welcome to ECALL services test"
string_buffer:
    .space      50
int_prompt:
    .string     "Enter an integer"
goodbye:
    .string     "All tests completed successfully"

