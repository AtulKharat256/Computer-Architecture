#BUBBLE SORT PROGRAM
#n will be in x3
lui x5 0x10001 #original array start address
lui x6 0x10002 #sorted array start address
lui x7 0x10000 #seed value stored in x7

.main:
#x28 is i
#x29 is 4*i
#x30 is Array[i]
#x31 is SortedArray[i]
#x9 is random()
add x28, x0, x0
.ml1start:
bge x28, x3, .ml1
jal x1, .random
addi x20, x0, 2
sll x29, x28, x20
add x30, x5, x29
add x31, x6, x29
sw x9, 0(x30)
sw x9, 0(x31)
addi x28, x28, 1
beq x0, x0, .ml1start
.ml1:
jal x1, .bubbleSort
beq x0, x0, .close

.random:
add x18, x0, x7
add x19, x0, x18
addi x20, x0, 21
sll x18, x18, x20
xor x18, x18, x19
add x19, x0, x18
addi x20, x0, 35
srl x18, x18, x20
xor x18, x18, x19
add x19, x0, x18
addi x20, x0, 4
sll x18, x18, x20
xor x18, x18, x19
add x7, x18, x0
add x9, x18, x0 #return value in x9
jalr x0, 0(x1) #returns to RA saved in x1

.bubbleSort:
#x18 is i
#x19 is j
#x3 is N
#x20 is n-i-1
#x21 SortedArray[j]
#x22 SortedArray[j+1]
#x23 is address of SortedArray[j]
#x24 is address of SortedArray[j+1]
#x25 is j*4
#x26 is temp1
#x27 is temp2
add x18, x0, x0
.l1start:
bge x18, x3, .loop1
add x19, x0, x0
sub x20, x3, x18
addi x20, x20, -1
.l2start: 
bge x19, x20, .loop2
addi x9, x0, 2
sll x25, x19, x9
add x23, x6, x25
lw x21, 0(x23)
addi x25, x25, 4
add x24, x6, x25
lw x22, 0(x24)
bge x21, x22, .noswap
#swap(j,j+1)
add x26, x21, x0
add x27, x22, x0
sw x26, 0(x24)
sw x27, 0(x23)
.noswap:
addi x19, x19, 1
beq x0, x0, .l2start
.loop2:
addi x18, x18, 1
beq x0, x0, .l1start
.loop1:
jalr x0, 0(x1) #returns to RA saved in x1

.close:
