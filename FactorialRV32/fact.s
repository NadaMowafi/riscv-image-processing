    .text
    .globl _start           # Entry point

_start:
    # Initialize registers
    li a0, 6                # Input number (e.g., 6)
    li a1, 1                # Initialize result (factorial) to 1
    li a2, 1                # Initialize loop counter to 1

factorial_loop:
    blt a0, a2, end         # If counter > input number, exit loop
    mul a1, a1, a2          # Multiply result by counter (a1 = a1 * a2)
    addi a2, a2, 1          # Increment counter (a2 = a2 + 1)
    j factorial_loop        # Repeat loop

end:
    # Store the result in memory
    li a3, 0x1000           # Memory address to store the result
    sw a1, 0(a3)            # Store the result (a1) at address 0x1000

    # Exit the program
    li a7, 93               # Exit system call number
    ecall                   # Execute system call
