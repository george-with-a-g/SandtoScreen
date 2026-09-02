@ examples/fibonacci.s
@ Computes the 10th Fibonacci number in Register R2

    MOV R0, #0           @ F(0) = 0
    MOV R1, #1           @ F(1) = 1
    MOV R3, #10          @ Loop counter N = 10

loop:
    ADD R2, R0, R1       @ F(n) = F(n-1) + F(n-2)
    MOV R0, R1           @ Shift F(n-2) = F(n-1)
    MOV R1, R2           @ Shift F(n-1) = F(n)
    SUBS R3, R3, #1      @ Decrement loop counter and update flags
    BNE loop             @ Loop while R3 != 0

halt:
    B halt               @ R2 now holds Fibonacci result!
