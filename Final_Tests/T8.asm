# ZX16 Edge Case Test Suite
# Tests various edge cases and error conditions for the ZX16 simulator

.text
.org 0x0020

main:
    # Test 1: Division by zero simulation (using repeated subtraction)
    li x1, 10           # Dividend
    li x2, 0            # Divisor (zero - should cause issues)
    call divide_test    # This should handle division by zero
    
    # Test 2: Memory boundary access - near upper limit
    li x1, 0x7FFE       # Near memory boundary
    li x2, 42
    sw x2, 0(x1)        # Store at boundary
    lw x3, 0(x1)        # Load from boundary
    
    # Test 3: Memory boundary access - at upper limit (should fail)
    li x1, 0x8000       # At memory boundary (MEM_SIZE)
    li x2, 99
    sw x2, 0(x1)        # This should cause out-of-bounds error
    
    # Test 4: Invalid register access simulation
    # Since we only have x0-x7, accessing beyond should be caught
    li x1, 0x1234
    add x1, x1          # Valid operation
    
    # Test 5: Stack overflow simulation
    li x1, 100          # Counter for deep recursion
    call deep_recursion # This will cause stack issues
    
    # Test 6: Invalid instruction simulation
    .word 0xFFFF        # Invalid instruction - should trigger unknown opcode
    
    # Test 7: Jump to invalid address
    li x1, 0x0010       # Invalid jump target (before .text section)
    jr x1               # Should cause invalid address warning
    
    # Test 8: Negative array index simulation
    li x1, 0x8000       # Base address
    li x2, -5           # Negative offset
    add x3, x1          # Calculate address
    add x3, x2          # x3 = base + (-5)
    lw x4, 0(x3)        # Load from potentially invalid address
    
    # Test 9: Integer overflow
    li x1, 0x7FFF       # Maximum positive 16-bit signed value
    li x2, 1
    add x1, x2          # Should overflow to negative
    
    # Test 10: Shift by invalid amount
    li x1, 0x1234
    li x2, 20           # Shift amount > 16 (invalid for 16-bit)
    sll x1, x2          # Should be masked to valid range
    
    # Test 11: Branch offset out of range simulation
    li x1, 5
    li x2, 5
    beq x1, x2, far_label  # This will test branch offset limits
    
    # Test 12: Uninitialized register usage
    add x7, x0          # x7 should be 0 initially
    add x6, x7          # Using potentially uninitialized register
    
    # Test 13: Ecall with invalid service number
    li x6, 999          # Invalid service number
    ecall 999           # Should trigger unknown ecall service error
    
    # Test 14: String print with invalid address
    li x6, 0x9000       # Address beyond memory bounds
    ecall 5             # print_string service - should fail
    
    # Test 15: Infinite loop detection test
    li x1, 1
infinite_loop:
    add x1, x1          # x1 = x1 + x1 (doubles each time)
    beq x1, x1, infinite_loop  # Always true - infinite loop
    
    # Normal exit
    clr x6
    ecall 3

# Division by zero test function
divide_test:
    # Simulate division using repeated subtraction
    add x3, x0          # Result counter
    add x4, x1          # Copy dividend
    
divide_loop:
    # Check if divisor is zero
    beq x2, x0, divide_by_zero
    
    # Check if dividend < divisor
    slt x5, x4, x2
    bne x5, x0, divide_done
    
    # Subtract divisor from dividend
    sub x4, x2
    addi x3, 1
    
    # Continue loop
    beq x0, x0, divide_loop
    
divide_by_zero:
    # Handle division by zero - set result to max value
    li x3, 0x7FFF
    
divide_done:
    add x1, x3          # Return result in x1
    ret

# Deep recursion test function
deep_recursion:
    # Decrement counter
    addi x1, -1
    
    # Base case
    beq x1, x0, recursion_done
    
    # Recursive call (will eventually overflow stack)
    call deep_recursion
    
recursion_done:
    ret

# Far label for branch offset test
.org 0x0200
far_label:
    li x1, 0xDEAD       # Marker value
    ret

# Test data section
.data
.org 0x8000
test_string: .ascii "Test String\0"
test_array: .word 1, 2, 3, 4, 5

# Invalid data to test edge cases
.org 0x8100
invalid_float: .word 0x3F80  # Represents float 1.0 in IEEE format
                              # ZX16 doesn't support floats - should be treated as integer

# Buffer overflow test data
.org 0x8200
small_buffer: .space 4       # Only 4 bytes allocated
overflow_data: .ascii "This string is much longer than the buffer can hold and will cause overflow"
