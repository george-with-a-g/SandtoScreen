# 🐍 Project 1: Writing an ARM Assembler in Python

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](../README.md) • [01. What is an ISA?](../01_what_is_an_isa.md) • [02. ARM Instruction Encoding](../02_arm_instruction_encoding.md) • [03. Pipeline Architecture](../03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./README.md) • [⚡ ARM7 Verilog CPU](../cpu_verilog/README.md) • [🚀 BootROM](../bootrom/README.md)
---

An **Assembler** is the foundational software tool that bridges the human world of text (`ADD R1, R2, #4`) and the physical silicon world of 32-bit binary integers (`0xE2821004`).

In this project, you explore the architecture of **`asm.py`** — an ARM7 (ARMv4T) assembler written from scratch in pure Python.

---

## 🏗️ The Two-Pass Assembler Architecture

Why does an assembler need **two passes** over the source code?

Consider this code:
```text
Address 0x00:   B loop_exit      @ Jump forward to a label we haven't seen yet!
Address 0x04:   ADD R0, R0, #1
Address 0x08: loop_exit:
Address 0x08:   MOV R1, #42
```

When the assembler is reading Line 1, it needs to calculate the branch offset to `loop_exit`. But `loop_exit` hasn't appeared yet in the file! This is the classic **Forward Reference Problem**.

To solve this, `asm.py` uses the standard **Two-Pass Algorithm**:

```
 SOURCE FILE (.s)
        │
        ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ PASS 1: Symbol Table & Address Calculation                  │
 │ • Strips whitespace and comments (@, //, ;)                 │
 │ • Tracks the Program Counter (PC = 0, 4, 8, ...)            │
 │ • Records all labels into a Symbol Table dictionary:        │
 │   labels["loop_exit"] = 0x00000008                          │
 └─────────────────────────────────────────────────────────────┘
        │
        ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ PASS 2: Machine Code Binary Emission                        │
 │ • Parses mnemonics (ADD, SUB, MOV, LDR, B, etc.)            │
 │ • Resolves label targets from the Symbol Table              │
 │ • Encodes Condition, Opcode, Rn, Rd, and Operand 2 bitfields│
 │ • Emits raw 32-bit binary (.bin) or Verilog hex (.hex)      │
 └─────────────────────────────────────────────────────────────┘
```

---

## 🚀 How to Run `asm.py`

### 1. Assemble to Raw Binary (`.bin`):
```bash
python3 asm.py examples/fibonacci.s -o fibonacci.bin
```

### 2. Assemble to Verilog Hex Text (`.hex` for `$readmemh`):
```bash
python3 asm.py examples/fibonacci.s -o fibonacci.hex -f hex
```

---

## 🔬 Dissecting the Code of `asm.py`

### 1. The Symbol Table in Pass 1:
```python
# Pass 1: Collect all labels
pc = 0
for line in lines:
    if ':' in line:
        label, line = line.split(':', 1)
        self.labels[label.strip()] = pc
    if line:
        pc += 4 # Every instruction takes 4 bytes in ARM
```

### 2. Computing Branch Offsets in Pass 2:
Because of the 3-stage CPU pipeline, ARM branch offsets are calculated relative to `PC + 8`:
```python
# Target address from symbol table
target_addr = self.labels[target_label]

# Subtract (PC + 8) and shift right by 2 (word offset)
word_offset = (target_addr - (pc + 8)) >> 2

# Pack into 24-bit field
word = (cond << 28) | (0b101 << 25) | (link_bit << 24) | (word_offset & 0x00FFFFFF)
```

---

## 🛠️ Step-by-Step Hands-On Tasks

---

### 🟢 Task 1: Assemble Your First Program
Inspect [`examples/add.s`](./examples/add.s):
```text
    MOV R0, #10
    MOV R1, #20
    ADD R2, R0, R1
    SUB R3, R2, #5
```

Assemble it to hex:
```bash
python3 asm.py examples/add.s -o add.hex -f hex
```

#### ❓ Task 1 Challenge:
> **Question:** Open `add.hex`. What is the 32-bit hex word generated for line 1 (`MOV R0, #10`)?  
> *(Test your prediction, then check Answer 1 in the Answer Key at the bottom!)*

---

### 🟢 Task 2: Write a Countdown Loop
Create a new file `countdown.s` that counts down from 5 to 0:
```text
    MOV R0, #5
loop:
    SUBS R0, R0, #1
    BNE loop
halt:
    B halt
```

#### ❓ Task 2 Challenge:
> **Question:** When `SUBS R0, R0, #1` decrements `R0` from 1 to 0, which condition flag causes `BNE` to stop branching?  
> *(Test your prediction, then check Answer 2 in the Answer Key at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why does `asm.py` advance the Program Counter by `+4` for every line of code?

### ❓ Quiz 2:
> If a branch jumps backward to a previous line (e.g. `B loop`), is the branch offset positive or negative?

### ❓ Quiz 3:
> What is the difference between an output `.bin` file and an output `.hex` file?

---

## 🎯 Summary Checklist: Assemblers

1. **Pass 1:** Strips comments, tracks PC addresses, and maps all label names to memory addresses.
2. **Pass 2:** Parses mnemonics and operands, computes branch distances (`target - (PC + 8)`), and encodes 32-bit binary words.
3. **Binary Emission:** Emits little-endian 32-bit machine code for RAM execution.

---

## 🔑 Answer Key & Deep Explanations

### Task Challenges:
* **Answer 1:** `E3A0000A`. (`Cond=E`, `Type=00`, `Imm=1`, `Opcode=MOV (D)`, `Rd=R0 (0)`, `Operand2=10 (0x0A)`).
* **Answer 2:** The **Zero (`Z`) Flag**. When `R0` reaches 0 (1 - 1 = 0), the subtraction sets `Z = 1`. The `BNE` (Branch if Not Equal) instruction only branches when `Z == 0`, so it falls through and exits the loop!

### Quizzes:
* **Answer Quiz 1:** Because in ARM, all instructions are fixed at exactly 32 bits (4 bytes) wide.
* **Answer Quiz 2:** **Negative!** It uses a 24-bit two's-complement negative offset (e.g. `0xFFFFFFFE` for a backward jump of 2 words).
* **Answer Quiz 3:** A `.bin` file contains raw binary bytes for physical hardware/flash memory. A `.hex` file contains ASCII hexadecimal strings (e.g. `E3A0000A`) designed for Verilog simulators via `$readmemh`.

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 3 Hub | ➡️ Next Project |
| :--- | :---: | ---: |
| [⬅️ 03. Pipeline Architecture](../03_cpu_pipeline_architecture.md) | [Section 3 Hub](../README.md) | [⚡ Project 2: ARM7 CPU in Verilog ➡️](../cpu_verilog/README.md) |
