# 02. ARM Instruction Encoding: How 32-Bit Machine Code is Packed

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](./README.md) • [01. What is an ISA?](./01_what_is_an_isa.md) • [02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) • [03. Pipeline Architecture](./03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./assembler/README.md) • [⚡ ARM7 Verilog CPU](./cpu_verilog/README.md) • [🚀 BootROM](./bootrom/README.md)
---

When you write an assembly line like `ADD R1, R2, #4`, how does the computer store that on disk or in RAM?

It stores it as a **single 32-bit binary integer** (4 bytes).

In this chapter, you will learn the exact bitfields that make up ARM32 machine code instructions so that you can:
1. Write the **Python Assembler** that generates these binary words.
2. Write the **Verilog Decoder** that extracts the fields to control the ALU and registers!

---

## 🧩 The Universal Anatomy of an ARM Instruction

Every 32-bit ARM instruction is laid out like this:

```
  31        28 27 26 25 24       21 20 19      16 15      12 11                      0
 ┌────────────┬─────┬───┬───────────┬───┬──────────┬──────────┬────────────────────────┐
 │ Condition  │ Typ │ I │  Opcode   │ S │    Rn    │    Rd    │       Operand 2        │
 │  (4 bits)  │(2b) │(1)│  (4 bits) │(1)│ (4 bits) │ (4 bits) │        (12 bits)       │
 └────────────┴─────┴───┴───────────┴───┴──────────┴──────────┴────────────────────────┘
```

---

## 🏷️ Field 1: Condition Codes (Bits 31 to 28)

The top 4 bits determine **under what conditions the instruction executes**:

| Condition Code | Suffix | Meaning | Flag Test |
| :---: | :---: | :--- | :--- |
| `1110` (`0xE`) | *(None)* / `AL` | **Always** (Default: execute unconditionally) | Any flag state |
| `0000` (`0x0`) | `EQ` | **Equal** | `Z == 1` (Zero flag set) |
| `0001` (`0x1`) | `NE` | **Not Equal** | `Z == 0` (Zero flag clear) |
| `1011` (`0xB`) | `LT` | **Less Than (Signed)** | `N != V` |
| `1100` (`0xC`) | `GT` | **Greater Than (Signed)** | `Z == 0` and `N == V` |

> In standard unconditional code (like `ADD R0, R1, R2`), the top 4 bits are **always `1110` (Hex `0xE`)**.

---

## 🧮 Group 1: Data Processing Instructions (`ADD`, `SUB`, `AND`, `ORR`, `MOV`, `CMP`)

These instructions perform arithmetic or logic inside the ALU:

```
  31        28 27 26  25  24     21  20  19      16 15      12 11                      0
 ┌────────────┬──────┬───┬──────────┬───┬──────────┬──────────┬────────────────────────┐
 │    Cond    │  00  │ I │  Opcode  │ S │    Rn    │    Rd    │       Operand 2        │
 └────────────┴──────┴───┴──────────┴───┴──────────┴──────────┴────────────────────────┘
```

* **`Cond [31:28]`**: Execution condition (e.g. `1110` for Always).
* **`00 [27:26]`**: Format identifier for Data Processing.
* **`I [25]` (Immediate Flag)**:
  * `0` = Operand 2 is a **Register** (`Rm`).
  * `1` = Operand 2 is an **Immediate Number** (e.g. `#4`).
* **`Opcode [24:21]`**: Which ALU math operation to perform:
  * `0000` = `AND`
  * `0001` = `EOR` (XOR)
  * `0010` = `SUB` (Subtract: `Rn - Operand2`)
  * `0100` = `ADD` (Add: `Rn + Operand2`)
  * `1010` = `CMP` (Compare: updates CPSR flags without saving result)
  * `1100` = `ORR` (Bitwise OR)
  * `1101` = `MOV` (Move / Copy: `Rd = Operand2`)
* **`S [20]` (Set Flags)**:
  * `1` = Update the `CPSR` condition flags (`NZCV`).
  * `0` = Leave flags unchanged.
* **`Rn [19:16]`**: First operand register (e.g. `R2` = `0010`).
* **`Rd [15:12]`**: Destination register where the answer is written (e.g. `R1` = `0001`).
* **`Operand 2 [11:0]`**: The second operand (8-bit immediate value with 4-bit rotate, or shifted register `Rm`).

---

### 🔍 Worked Example 1: Encoding `ADD R1, R2, #4`

Let's assemble `ADD R1, R2, #4` bit-by-bit into 32-bit machine code:

```text
Instruction:  ADD R1, R2, #4
Meaning:      R1 = R2 + 4

1. Cond       = 1110   (Always execute)
2. Type       = 00     (Data Processing)
3. I (Imm)    = 1      (Second operand is a constant number: #4)
4. Opcode     = 0100   (ADD operation)
5. S (Flags)  = 0      (Do not update CPSR flags)
6. Rn         = 0010   (Source register R2)
7. Rd         = 0001   (Destination register R1)
8. Operand 2  = 0000 0000 0100 (Immediate number 4)

Binary:  1110 00 1 0100 0 0010 0001 000000000100
Groups:  1110  0010  1000  0010  0001  0000  0000  0100
Hex:     0x   E     2     8     2     1     0     0     4

Final Machine Code: 0xE2821004 ✅
```

---

## 📦 Group 2: Memory Access (`LDR` and `STR`)

These instructions read from or write to RAM:

```
  31        28 27 26  25  24 23 22 21 20 19      16 15      12 11                      0
 ┌────────────┬──────┬───┬──┬──┬──┬──┬──┬──────────┬──────────┬────────────────────────┐
 │    Cond    │  01  │ I │ P│ U│ B│ W│ L│    Rn    │    Rd    │       Offset           │
 └────────────┴──────┴───┴──┴──┴──┴──┴──┴──────────┴──────────┴────────────────────────┘
```

* **`01 [27:26]`**: Format identifier for Single Data Transfer.
* **`L [20]` (Load / Store)**:
  * **`1` = `LDR` (Load from Memory into Register `Rd`)**
  * **`0` = `STR` (Store from Register `Rd` into Memory)**
* **`B [22]` (Byte / Word)**:
  * `0` = 32-bit Word access.
  * `1` = 8-bit Byte access (`LDRB` / `STRB`).
* **`Rn [19:16]`**: Base address register in RAM.
* **`Rd [15:12]`**: Source / Destination register.
* **`Offset [11:0]`**: 12-bit memory address offset (e.g. `[Rn, #4]`).

---

## 🔀 Group 3: Branch Instructions (`B` and `BL`)

Used for loops, if-statements, and function calls:

```
  31        28 27   25  24  23                                                     0
 ┌────────────┬───────┬───┬────────────────────────────────────────────────────────┐
 │    Cond    │  101  │ L │                  24-Bit Signed Offset                  │
 └────────────┴───────┴───┴────────────────────────────────────────────────────────┘
```

* **`101 [27:25]`**: Format identifier for Branch.
* **`L [24]` (Link Flag)**:
  * `0` = Plain jump (`B loop_start`).
  * `1` = **Branch with Link (`BL my_function`)**: saves return address into `LR` (`R14`).
* **`Offset [23:0]`**: A 24-bit signed integer word offset. 
  * Because all instructions are 4 bytes aligned, the CPU shifts this offset **left by 2 bits** (Offset * 4), allowing a jump range of +/- 32 MB!

---

## 🛠️ Step-by-Step Hands-On Exercises

---

### 🟢 Exercise 1: Encoding a Register Move (`MOV R0, #10`)
Let's assemble `MOV R0, #10`:
* Condition = `1110` (Always)
* Type = `00`
* `I = 1` (Immediate `#10`)
* Opcode = `1101` (`MOV`)
* `S = 0`
* `Rn = 0000` (Ignored for MOV)
* `Rd = 0000` (`R0`)
* `Operand2 = 0x00A` (Decimal 10)

#### ❓ Question 1:
What is the 32-bit hexadecimal machine code word for `MOV R0, #10`?  
*(Test your prediction, then check Answer 1 at the bottom!)*

---

### 🟢 Exercise 2: Decoding Machine Code
Suppose the CPU reads the 32-bit word **`0xE0810002`** from RAM:
```text
Hex:    E    0    8    1    0    0    0    2
Binary: 1110 00 0 0100 0 0001 0000 0000 0010
```

* `Cond = 1110` (Always)
* `I = 0` (Operand 2 is a register)
* `Opcode = 0100` (`ADD`)
* `Rn = 0001` (`R1`)
* `Rd = 0000` (`R0`)
* `Operand 2 = R2`

#### ❓ Question 2:
What is the human-readable assembly instruction for `0xE0810002`?  
*(Test your prediction, then check Answer 2 at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why does the immediate field `I [25]` matter so much to the CPU's Instruction Decoder?

### ❓ Quiz 2:
> In the branch instruction `B target`, why is the 24-bit offset shifted left by 2 bits when added to the PC?

### ❓ Quiz 3:
> What happens to the CPSR flags if you run `ADD R0, R1, R2` versus `ADDS R0, R1, R2`?

---

## 🎯 Summary Checklist: Instruction Encoding

1. **Fixed 32 Bits:** Every ARM instruction is packed into a single 32-bit binary integer.
2. **Top 4 Bits (31:28):** Condition code (`0xE` = Always).
3. **Opcode (24:21):** Tells the ALU which mathematical circuit to activate.
4. **Registers (Rn & Rd):** 4 bits each, selecting from `R0` (0000) to `R15` (1111).
5. **The Assembler's Job:** Turn strings of assembly text into these exact 32-bit binary words.

---

## 🔑 Answer Key & Deep Explanations

### Exercise Challenges:
* **Answer 1:** **`0xE3A0000A`**.
  * `1110 (E)` `0011 (3)` `1010 (A)` `0000 (0)` `0000 (0)` `0000 (0)` `0000 (0)` `1010 (A)`
* **Answer 2:** **`ADD R0, R1, R2`** (Adds `R1 + R2` and stores result in `R0`).

### Quizzes:
* **Answer Quiz 1:** `I [25]` tells the decoder whether the second input to the ALU comes from the **Register File** (`I = 0`) or directly from the **Instruction Word Immediate bits** (`I = 1`).
* **Answer Quiz 2:** Because all 32-bit ARM instructions are aligned to 4-byte boundaries, the bottom 2 bits of any instruction address are always `00`. Shifting left by 2 (Offset * 4) allows a 24-bit field to address a 26-bit range (+/- 32 MB) without wasting bits!
* **Answer Quiz 3:** `ADD` has `S = 0`, so it does **not** modify condition flags. `ADDS` has `S = 1`, so the ALU updates the `N`, `Z`, `C`, and `V` flags in `CPSR` based on the addition result.

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 3 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ 01. What is an ISA?](./01_what_is_an_isa.md) | [Section 3 Hub](./README.md) | [03. Pipeline Architecture ➡️](./03_cpu_pipeline_architecture.md) |
