.section .data
.align 4
vec1:
    .hword 1, 2, 3, 4, 5, 6
vec2:
    .hword 7, 8, 9, 10, 11, 12

.section .text
.globl _start
_start:
    vsetivli x0, 6, e16, m2   #Set VL=6 , LMUL=2, SEW=16,x0 dummy not save vl
    la t1,vec1  #load address  of vec1 to t1 
    vle16.v v0, (t1)   #Store first vector from this address
    la t2,vec2  #Load address of vec2 to t2
    vle16.v v2, (t2)   # Sttore second vector from this address
    vmul.vv v4, v0, v2     #multiply both
    vsetivli x0, 6, e16, m1 #Set  LMUL=1 as redsum ret to scalar
    vmv.s.x v5, x0  #Initialize v5 =0
    vredsum.vs v5, v4, v5   #Sum of All elements of V4 with V5 
    vmv.x.s x18, v5  #Store it to x18
    li a7, 93     #Exit
    ecall

