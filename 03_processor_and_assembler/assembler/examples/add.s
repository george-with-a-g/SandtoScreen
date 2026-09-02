# examples/add.s
@ Basic Arithmetic Test in ARM Assembly

    MOV R0, #10          @ R0 = 10
    MOV R1, #20          @ R1 = 20
    ADD R2, R0, R1       @ R2 = R0 + R1 (30)
    SUB R3, R2, #5       @ R3 = R2 - 5  (25)
    CMP R2, #30          @ Compare R2 with 30 (Z flag becomes 1)
    BEQ success          @ Branch to success if Equal!

fail:
    MOV R4, #0           @ Failed flag
    B done

success:
    MOV R4, #1           @ Success flag = 1!

done:
    B done               @ Infinite loop
