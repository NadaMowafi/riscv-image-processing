# fibonacci.s
# RV32I assembly program to compute Fibonacci series

.global _start
_start:
    li t0, 0          # t0 = Fibonacci(0) = 0
    li t1, 1          # t1 = Fibonacci(1) = 1
    li t2, 10         # t2 = Number of Fibonacci numbers to compute
    li t3, 0          # t3 = Loop counter

fibonacci_loop:
    mv t4, t1         # t4 = t1 (save t1)
    add t1, t0, t1    # t1 = t0 + t1 (next Fibonacci number)
    mv t0, t4         # t0 = t4 (update t0 to previous t1)
    addi t3, t3, 1    # Increment loop counter
    blt t3, t2, fibonacci_loop  # Repeat loop if t3 < t2

    # Exit the program
    li a7, 93         # Exit syscall number
    ecall             # Perform syscall
