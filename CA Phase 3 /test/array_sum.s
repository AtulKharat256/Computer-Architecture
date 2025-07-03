#SUM OF ARRAY OF N ELEMENTS
#x3 contains value of N

lui x9 0x10001 #x9 stores address of array root

add x5 x9 x0 #temp
add x6 x0 x0 #i= 0
loop:
bge x6 x3 tempreset
sw x6 0(x5)
addi x5 x5 4
addi x6 x6 1 #i= i+1
beq x0 x0 loop
tempreset:
add x5 x9 x0
add x6 x0 x0 #i= 0
add x7 x0 x0 #arr[i]= 0
add x8 x0 x0 #SUM= 0
sum:
bge x6 x3 exit
lw x7 0(x5)
add x8 x8 x7 #SUM= SUM+arr[i]
addi x5 x5 4
addi x6 x6 1 #i= i+1
beq x0 x0 sum
exit:
sw x8 0(x5)
