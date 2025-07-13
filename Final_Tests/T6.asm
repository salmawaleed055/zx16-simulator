# zx16 system call and pseudo-instructions test
# tests ecall and various pseudo-instructions

.text
.org 0x0000
main:

    # inc / dec
    li  x1, 5
    inc x1              # x1 = 6
    dec x1              # x1 = 5
    li  x2, 5
    bne x1, x2, test_failed

    # neg (two's-complement)
    li  x3, 10
    neg x3              # x3 = -10
    li  x4, -10
    bne x3, x4, test_failed

    # not (bitwise complement)
    li  x5, 0x00ff
    not x5              # x5 = 0xff00
    li  x6, 0xff00
    bne x5, x6, test_failed

    # clr (clear register)
    li  x7, 0x1234
    clr x7              # x7 = 0
    bnz x7, test_failed

    # nop (no operation)
    nop                 # should do nothing

    # test function-call pseudo-instructions
    li   x0, 0
    call test_function  # should increment x0
    li   x1, 1
    bne  x0, x1, test_failed

    # system-call placeholders
    # ecall 0x000        # print char  (if supported)
    # ecall 0x001        # read  char  (if supported)
    # ecall 0x002        # print string(if supported)

    j test_passed

test_function:
    inc x0              # increment x0
    ret                 # return

test_failed:
    clr a0
    ecall 3             # exit with failure

test_passed:
    li  a0, 1
    ecall 3             # exit with success
