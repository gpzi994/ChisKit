.section .entry57,"ax",@progbits
.global read57_entry
read57_entry:
    beqz a2, .Lrange57
    li t0, 2
    bne a0, t0, .Lrange57
    andi t0, a1, 31
    bnez t0, .Lrange57
    li t0, 0x01ffffe0
    bltu t0, a1, .Lrange57
    li t0, 32
    bne a3, t0, .Lrange57
    li t0, 50000
    bne a4, t0, .Lrange57
    li a0, 0
    j xfer57
.Lrange57:
    li a0, -11
    ret

.section .space57,"ax",@progbits
.global space57_entry
space57_entry:
    bnez a1, ident57
    mv a1, a2
    mv a2, a3
    mv a3, a4
    mv a4, a5
    j read57_entry
