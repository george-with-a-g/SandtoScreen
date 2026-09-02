# 🚀 Project 3: Writing the BootROM in Assembly

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](../README.md) • [01. What is an ISA?](../01_what_is_an_isa.md) • [02. ARM Instruction Encoding](../02_arm_instruction_encoding.md) • [03. Pipeline Architecture](../03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](../assembler/README.md) • [⚡ ARM7 Verilog CPU](../cpu_verilog/README.md) • [🚀 BootROM](./README.md)
---

When an iPhone, a game console, or a computer first receives power, its RAM is completely empty (filled with random garbage voltages).

How does the processor know what to do?

Every CPU contains a tiny, permanently etched program called the **BootROM**. In this project, you inspect and assemble **`bootrom.s`** — a 25-instruction bare-metal bootloader that downloads programs over the UART serial line into RAM and jumps to them!

---

## 🔌 The Boot Process Explained

```
 Power Applied (Reset Released)
               │
               ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. CPU boots at Address 0x00000000 (Inside BootROM)         │
 │ • Initializes UART pointers (0x10000000)                    │
 │ • Sets RAM destination pointer to 0x00001000                │
 │ • Sends '?' over UART to signal: "READY FOR PROGRAM"        │
 └─────────────────────────────────────────────────────────────┘
               │
               ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 2. Serial Download Loop                                     │
 │ • Polls UART Status Register for incoming bytes             │
 │ • Stores each received byte into RAM: STRB R0, [R6]         │
 │ • Increments RAM pointer: ADD R6, R6, #1                    │
 │ • Echoes byte back to terminal for verification             │
 └─────────────────────────────────────────────────────────────┘
               │
               ▼ (User sends '!' to signal End of File)
 ┌─────────────────────────────────────────────────────────────┐
 │ 3. 🚀 Jump to User Program                                  │
 │ • Executes: MOV PC, R6 (R6 = 0x00001000)                    │
 │ • Control transfers to user software running in RAM!        │
 └─────────────────────────────────────────────────────────────┘
```

---

## 📜 Dissecting `bootrom.s`

### 1. Register Setup
```verilog
MOV R4, #0x10000000        @ R4 = UART Data Register (MMIO)
ADD R5, R4, #4             @ R5 = UART Status Register (0x10000004)
MOV R6, #0x1000            @ R6 = Destination address in RAM (0x00001000)
```

### 2. Polling UART Status Register
```verilog
poll_rx:
    LDR R1, [R5]               @ Read UART Status (Bit 1 = RX Data Valid)
    TST R1, #2                 @ Test if bit 1 is set
    BEQ poll_rx                @ If 0 (no data), keep polling!

    LDRB R0, [R4]              @ Read incoming character
```

### 3. Writing into RAM and Jumping
```verilog
    CMP R0, #33                @ Did the user send '!' (ASCII 33)?
    BEQ boot_user_code

    STRB R0, [R6]              @ Store byte into RAM!
    ADD R6, R6, #1             @ Advance destination pointer
    B wait_for_program

boot_user_code:
    MOV PC, R6                 @ 🚀 Jump to RAM (0x00001000)!
```

---

## 🛠️ Step-by-Step Hands-On Tasks

---

### 🟢 Task 1: Assemble the BootROM
Assemble `bootrom.s` into hex format for Verilog initialization:
```bash
python3 ../assembler/asm.py bootrom.s -o bootrom.hex -f hex
```

#### ❓ Task 1 Challenge:
> **Question:** How many 32-bit instruction words does the BootROM require?  
> *(Test your prediction, then check Answer 1 in the Answer Key at the bottom!)*

---

### 🟢 Task 2: Understanding MMIO Polling
Look at the polling loop:
```verilog
poll_rx:
    LDR R1, [R5]
    TST R1, #2
    BEQ poll_rx
```

#### ❓ Task 2 Challenge:
> **Question:** What does the `TST R1, #2` instruction do to determine if a byte is ready?  
> *(Test your prediction, then check Answer 2 in the Answer Key at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why is the BootROM stored at address `0x00000000` rather than higher in memory?

### ❓ Quiz 2:
> Why does the BootROM download code to address `0x00001000` instead of overwriting `0x00000000`?

### ❓ Quiz 3:
> How does the instruction `MOV PC, R6` cause execution to jump to a new memory address?

---

## 🎯 Summary Checklist: BootROM & Bringup

1. **Power-On Reset:** The CPU initializes `PC = 0x00000000` where the BootROM resides.
2. **UART Handshake:** BootROM sends `?` to signal the host PC that it is ready to receive binary data.
3. **RAM Download:** Incoming stream is written into RAM via `STRB`.
4. **Handoff:** Setting `PC = 0x00001000` transfers control to the loaded application.

---

## 🔑 Answer Key & Deep Explanations

### Task Challenges:
* **Answer 1:** Exactly **25 instructions (100 bytes)**! It easily fits within a tiny fraction of an FPGA Block RAM.
* **Answer 2:** It performs a bitwise AND between `R1` and `2` (`0b0010`). If Bit 1 (RX Data Valid) is `0`, the result is zero, setting the `Z` flag and causing `BEQ` to loop. When Bit 1 becomes `1`, `Z = 0` and the loop exits.

### Quizzes:
* **Answer Quiz 1:** Because on reset, ARM processors hardware-initialize the Program Counter (`PC`) to `0x00000000`.
* **Answer Quiz 2:** Because `0x00000000` is the BootROM itself! Writing user code to `0x00001000` leaves the BootROM code intact so the system can be reset and re-flashed without power-cycling.
* **Answer Quiz 3:** In ARM, the Program Counter (`PC`) is accessible as register `R15`. Writing an address to `PC` forces the Fetch stage to immediately begin reading from that new address on the next clock cycle!

---

## 🧭 Navigation
| ⬅️ Previous Project | 🏠 Section 3 Hub | ➡️ Next Section |
| :--- | :---: | ---: |
| [⬅️ ⚡ Project 2: ARM7 CPU in Verilog](../cpu_verilog/README.md) | [Section 3 Hub](../README.md) | [Section 4: Compiler (A High-Level Language) ➡️](../../README.md) |
