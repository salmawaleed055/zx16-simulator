.org 0x0000
.text
main:
li a0, 5
ecall 1
mv t0, a0
sll t0, a0
slt a0, t0
ecall 1
sub t0, a0
or a0, t0
ecall 1

ecall 3