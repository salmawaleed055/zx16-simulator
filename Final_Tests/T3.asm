# ZX16 Branch Instructions Test
# Tests all conditional branch operations

.text
.org 0x0000

main:

    # Test BEQ (branch if equal)
    li x1, 5
    li x2, 5
    beq x1, x2, beq_taken
    j test_failed       # Should not reach here

beq_taken:
    # Test BNE (branch if not equal)
    li x3, 10
    li x4, 15
    bne x3, x4, bne_taken
    j test_failed

bne_taken:
    # Test BZ (branch if zero)
    clr x5
    bz x5, bz_taken
    j test_failed

bz_taken:
    # Test BNZ (branch if not zero)
    li x6, 1
    bnz x6, bnz_taken
    j test_failed

bnz_taken:
    # Test BLT (branch if less than, signed)
    li x1, -5
    li x2, 3
    blt x1, x2, blt_taken
    j test_failed

blt_taken:
    # Test BGE (branch if greater or equal, signed)
    li x3, 10
    li x4, 10
    bge x3, x4, bge_taken
    j test_failed

bge_taken:
    # Test BLTU (branch if less than, unsigned)
    li x5, 5
    li x6, 10
    bltu x5, x6, bltu_taken
    j test_failed

bltu_taken:
    # Test BGEU (branch if greater or equal, unsigned)
    li x7, 0xFF
    li x0, 100
    bgeu x7, x0, bgeu_taken
    j test_failed

bgeu_taken:
    # Test forward and backward branches
    li x1, 0
    j forward_test

backward_target:
    inc x1
    bz x1, test_failed  # x1 should be 1, not 0
    j test_passed

forward_test:
    bz x1, backward_target
    j test_failed

test_passed:
    li a0, 1            # Success code
    ecall 3

test_failed:
    clr a0              # Failure code
    ecall 3
