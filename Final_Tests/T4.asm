# ZX16 J-Type Instructions Test

.text
.org 0x0000
main:
    li x1, 0
    j jump_target
    inc x1              # Should not execute
    j test_failed

jump_target:
    bnz x1, test_failed # x1 should still be 0

    # Test JAL (jump and link)
    clr x2
    jal x1, subroutine  # Call subroutine, return address in x1
    li x3, 1
    bne x2, x3, test_failed # x2 should be 1 after subroutine

    j test_passed

subroutine:
    inc x2              # Increment x2
    jr x1                 # Return (JR x1)

test_failed:
    clr a0
    ecall 3

test_passed:
    li a0, 1
    ecall 3
