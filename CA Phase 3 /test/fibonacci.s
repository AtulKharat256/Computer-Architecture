#FIBONACCI PROGRAM
#x3 contains value of n
add x1, x0, x0
addi x2, x0, 1
bne x3, x0, nzero
add x2, x0, x1
nzero: 
addi x5, x0, 2
loop: blt x3, x5, ndone
add x4, x1, x2
add x1, x0, x2
add x2, x0, x4
addi x5, x5, 1
beq x0, x0, loop
ndone:
