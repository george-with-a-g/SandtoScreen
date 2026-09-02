@ bootrom.s — ARM7 Bare-Metal BootROM (~40 Lines)
@ Embedded into the FPGA at power-on (Address 0x00000000).
@ Listens on the UART serial line, downloads user programs into RAM (0x00001000),
@ and jumps execution to the downloaded code!

_start:
    @ Initialize Pointers
    MOV R4, #0x10000000        @ R4 = UART Data Register Address (0x10000000)
    ADD R5, R4, #4             @ R5 = UART Status Register Address (0x10000004)
    MOV R6, #0x1000            @ R6 = Destination address in RAM (0x00001000)
    MOV R7, #0                 @ R7 = Byte counter

    @ Send Prompt Character '?' over UART
    MOV R0, #63                @ ASCII '?' = 63
    BL uart_putc

wait_for_program:
    @ Poll UART Status Register until a byte arrives
poll_rx:
    LDR R1, [R5]               @ Read UART Status
    TST R1, #2                 @ Test Bit 1 (RX Data Valid)
    BEQ poll_rx                @ If 0, keep waiting!

    @ Read the received byte from UART Data Register
    LDRB R0, [R4]              @ Read incoming byte

    @ Check if this is the End-of-Transmission (Trigger Execution on '!')
    CMP R0, #33                @ ASCII '!' = 33
    BEQ boot_user_code

    @ Store received byte into RAM and advance pointer
    STRB R0, [R6]              @ Write byte into RAM at [R6]
    ADD R6, R6, #1             @ Advance RAM destination address
    ADD R7, R7, #1             @ Increment byte counter

    @ Echo character back to confirm receipt
    BL uart_putc
    B wait_for_program

boot_user_code:
    @ Send Confirmation '!' over UART
    MOV R0, #33                @ ASCII '!'
    BL uart_putc

    @ 🚀 JUMP EXECUTION TO DOWNLOADED CODE IN RAM!
    MOV PC, R6

@ ----------------------------------------------------------------------------
@ Helper Function: Send 1 Byte in R0 over UART
@ ----------------------------------------------------------------------------
uart_putc:
    LDR R1, [R5]               @ Read UART Status
    TST R1, #1                 @ Test Bit 0 (TX Ready)
    BEQ uart_putc              @ If busy, keep waiting
    STRB R0, [R4]              @ Write byte to UART TX register
    MOV PC, LR                 @ Return from function
