# 01. What is an ISA? The ARM7 Architecture

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](./README.md) • [00. Assembly from Scratch](./00_assembly_from_scratch.md) • [01. What is an ISA?](./01_what_is_an_isa.md) • [02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) • [03. Pipeline Architecture](./03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./assembler/README.md) • [⚡ ARM7 Verilog CPU](./cpu_verilog/README.md) • [🚀 BootROM](./bootrom/README.md)
---

When programmers talk about a computer, they often think of high-level code: `if (x == 5) { y = 10; }`.

A silicon chip, however, cannot read text. A CPU is a collection of transistors that only knows how to perform a fixed set of basic operations (like *"add two numbers"*, *"load a byte from memory"*, or *"jump to an address"*).

The formal specification that defines **which operations a CPU can perform, how registers are organized, and how machine code instructions are encoded** is called the **Instruction Set Architecture (ISA)**.

---

## 📜 The Restaurant Menu Analogy

Think of an ISA as the **Legal Contract / Menu** between software and hardware:

```
  SOFTWARE (The Customer)                  HARDWARE (The Kitchen)
  ┌─────────────────────────┐              ┌─────────────────────────┐
  │ Reads the ISA Menu      │              │ Cooks the ordered meal  │
  │ Orders: "ADD R1, R2, R3"├─────────────►│ Activates ALU adder    │
  │ Orders: "LDR R0, [R4]"  │ (Machine     │ Reads RAM memory        │
  │ Orders: "B loop_start"  │  Code Word)  │ Updates Program Counter │
  └─────────────────────────┘              └─────────────────────────┘
                                   ▲
                                   │
                      THE INSTRUCTION SET (ISA)
                      "The Contract Between Both"
```

* **Software engineers and compilers (GCC, LLVM, Python)** write code that adheres to the ISA.
* **Hardware chip architects (Intel, ARM, Apple)** design circuits that physically implement the ISA.
* As long as both follow the contract, software written today can run on chips manufactured 20 years in the future!

---

## 🥊 RISC vs. CISC: The Two Philosophies

In computer history, two major philosophies emerged for designing an ISA:

| Feature | **CISC (Complex Instruction Set)** | **RISC (Reduced Instruction Set)** |
| :--- | :--- | :--- |
| **Philosophy** | "Provide single complex instructions that do everything" | "Provide simple, fast, single-cycle building blocks" |
| **Famous Examples** | Intel / AMD **x86** (PCs, Laptops, Servers) | **ARM**, **RISC-V**, **MIPS** (iPhones, Androids, Apple Silicon M1/M2/M3) |
| **Instruction Length**| Variable (1 byte to 15 bytes long!) | **Fixed** (Every instruction is exactly 32 bits / 4 bytes) |
| **Memory Access** | Almost any instruction can read/write RAM directly | **Load/Store Architecture** (Only `LDR` and `STR` touch RAM) |
| **Hardware Complexity**| Very high (requires massive microcode decoders) | Clean, small silicon die, ultra-low power consumption |

> **Why ARM / RISC?** In this course, we build an **ARM7 (ARMv4T)** core. Because RISC instructions are all exactly 32 bits wide, building the Verilog decoder and pipeline is clean, elegant, and directly teaches how modern processors (like Apple M-series chips) are engineered.

---

## 🏛️ The 16 ARM Core Registers (`R0` – `R15`)

As introduced in [00. Assembly from Scratch](./00_assembly_from_scratch.md), the CPU keeps 16 high-speed sticky notes right on its desk. 

In physical silicon, each of these 16 registers is **literally 32 D Flip-Flops wired side-by-side** to store a 32-bit number. The hardware component that holds them all is called the **Register File** (`register_file.v`):

```
                  ┌──────────────────────────────────────────────┐
                  │          THE ARM 32-BIT REGISTER FILE        │
                  ├───────────┬──────────────────────────────────┤
                  │  R0 – R3  │ Function Arguments & Math Scratch│
                  │  R4 – R11 │ General-Purpose Variable Storage │
                  │  R12 (IP) │ Temporary Scratchpad Register    │
                  ├───────────┼──────────────────────────────────┤
                  │  R13 (SP) │ Stack Pointer (Points to RAM)    │
                  │  R14 (LR) │ Link Register (The Bookmark)     │
                  │  R15 (PC) │ Program Counter (Current Pointer)│
                  └───────────┴──────────────────────────────────┘
```

### The 3 Special Purpose Registers:

1. **`R13 / SP` (Stack Pointer):**
   * A memory address pointing to a scratchpad area in RAM. When the CPU runs out of registers to hold variables, it temporarily spills extra data into RAM at the address pointed to by `SP`.

2. **`R14 / LR` (Link Register / The Bookmark):**
   * Think of `LR` as an automatic bookmark! When you jump to a subroutine function (`BL my_function`), the CPU automatically saves the address of the next instruction into `LR`. When your function is done, jumping right back to where you left off is as simple as: `MOV PC, LR`!

3. **`R15 / PC` (Program Counter):**
   * A finger pointing at the instruction currently being executed in RAM. As the clock ticks, the `PC` automatically advances by `4` on every step (since every ARM instruction is 4 bytes wide).

---

## 🚩 The Current Program Status Register (`CPSR`)

Along with the 16 registers, the CPU contains a special status register named **`CPSR`**. 

The top 4 bits of `CPSR` are the **Scoreboard Flags (NZCV)**. Whenever you run arithmetic or comparisons (`CMP`, `ADDS`, `SUBS`), the ALU automatically flips these 4 scoreboard lights:

```
  Bit 31       Bit 30       Bit 29       Bit 28             Bits 27 ... 0
 ┌────────────┬────────────┬────────────┬────────────┬────────────────────────┐
 │   N Flag   │   Z Flag   │   C Flag   │   V Flag   │   Control / Mode Bits  │
 │ (Negative) │   (Zero)   │  (Carry)   │ (oVerflow) │   (ARM vs Thumb mode)  │
 └────────────┴────────────┴────────────┴────────────┴────────────────────────┘
```

### 💡 Demystifying the 4 Flags:

In binary hardware, the **Most Significant Bit (Bit 31)** represents the sign:
* If Bit 31 is **`0`**, the number is **Positive**.
* If Bit 31 is **`1`**, the number is **Negative** (Two's Complement).

| Flag | Name | Plain English Meaning | Concrete Math Example |
| :---: | :--- | :--- | :--- |
| **`Z`** | **Zero** | The result was **exactly zero**. | `CMP R0, R1` when `R0 = 5` and `R1 = 5`. Since `5 - 5 = 0`, **`Z = 1`**. |
| **`N`** | **Negative** | The result was **negative** (Bit 31 is `1`). | `3 - 5 = -2`. In 32-bit binary, `-2` has Bit 31 set to `1`, so **`N = 1`**. |
| **`C`** | **Carry** | Unsigned math **rolled past the limit** (like a car odometer rolling past 999,999). | Adding `1` to `0xFFFFFFFF` (4.2 billion) wraps around to `0`, so **`C = 1`**. |
| **`V`** | **Overflow** | Signed math **accidentally flipped signs**! (Positive + Positive = Negative). | Adding two huge positive numbers spills into Bit 31, making the answer negative! **`V = 1`**. |

---

## ⚡ The Superpower of ARM: Conditional Execution

In x86 or MIPS, if you want to execute an instruction conditionally, you must write a branch jump (`if x == 0 jump to label`).

In ARM, **EVERY SINGLE INSTRUCTION CAN BE CONDITIONAL!**

You simply attach a 2-letter condition suffix to any instruction:
* **`ADDEQ R0, R1, R2`** --> Only add if the **Zero flag is 1 (Equal)**!
* **`MOVNE R0, #1`** --> Only move if **Not Equal (`Z == 0`)**!
* **`SUBLT R0, R0, #1`** --> Only subtract if **Less Than (`N != V`)**!

If the condition test fails in hardware, the CPU treats the instruction as a `NOP` (No Operation) and skips it in one clock cycle with zero branch penalty!

---

## 🛠️ Step-by-Step Hands-On Exercises

---

### 🟢 Exercise 1: Tracing the Program Counter
In 32-bit ARM, every instruction occupies 4 bytes in RAM.

Suppose the CPU boots with the Program Counter `PC = 0x00000000`.

```text
Address 0x00000000:   MOV R0, #5
Address 0x00000004:   MOV R1, #10
Address 0x00000008:   ADD R2, R0, R1
```

#### ❓ Question 1:
After executing `MOV R0, #5` and `MOV R1, #10`, what is the address in the `PC` when fetching the `ADD` instruction?  
*(Test your prediction, then check Answer 1 at the bottom!)*

---

### 🟢 Exercise 2: Tracing Condition Flags
Suppose `R0 = 15` and `R1 = 15`. The CPU executes:
```text
CMP R0, R1
```
*(Remember: `CMP` subtracts the two numbers: `R0 - R1 = 15 - 15 = 0` and updates flags!)*

#### ❓ Question 2:
What is the value of the **`Z` (Zero)** flag after this instruction?  
*(Test your prediction, then check Answer 2 at the bottom!)*

---

### 🟢 Exercise 3: Function Calling with `LR`
A program calls a subroutine located at address `0x00000100` using:
```text
Address 0x00000020:   BL my_subroutine
```

#### ❓ Question 3:
What return address does the hardware automatically save into `R14 (Link Register / LR)`?  
*(Test your prediction, then check Answer 3 at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why are all ARM instructions exactly 32 bits (4 bytes) wide, whereas x86 instructions can range anywhere from 1 to 15 bytes?

### ❓ Quiz 2:
> In ARM, can an arithmetic instruction like `ADD R0, R1, [0x1000]` read directly from a memory address in RAM?

### ❓ Quiz 3:
> What happens if an instruction with the suffix `EQ` (e.g. `ADDEQ R0, R1, R2`) is executed when the Zero flag `Z` is `0`?

---

## 🎯 Summary Checklist: The ARM ISA

1. **RISC Design:** Fixed 32-bit instructions, clean Load-Store memory architecture.
2. **16 Registers:** `R0`–`R12` general purpose, `R13=SP`, `R14=LR`, `R15=PC`.
3. **CPSR Flags:** `N` (Negative), `Z` (Zero), `C` (Carry), `V` (Overflow).
4. **Conditional Execution:** Any instruction can be conditionally executed based on CPSR flags (`EQ`, `NE`, `LT`, `GT`).

---

## 🔑 Answer Key & Deep Explanations

### Exercise Challenges:
* **Answer 1:** `PC = 0x00000008`. Each 32-bit instruction is 4 bytes wide, so the PC advances: `0x00 -> 0x04 -> 0x08`.
* **Answer 2:** `Z = 1`. Because 15 - 15 = 0, the result is zero, which asserts the Zero flag `Z` to `1`.
* **Answer 3:** `LR = 0x00000024`. `BL` saves the address of the *next sequential instruction* (0x20 + 4 = 0x24) into the Link Register so the function knows where to return when it finishes.

### Quizzes:
* **Answer Quiz 1:** ARM uses a RISC architecture where fixed 32-bit instructions make hardware decoders simple, fast, and easy to pipeline. x86 is a CISC architecture designed in the 1970s that packed variable-length instructions to save memory space.
* **Answer Quiz 2:** **No!** ARM is a strict **Load/Store Architecture**. Arithmetic instructions only operate on registers (`ADD R0, R1, R2`). To read RAM, you must first load it into a register with `LDR`.
* **Answer Quiz 3:** The hardware evaluates the condition, sees that `Z == 0` (False), cancels the operation, and skips the addition in 1 clock cycle like a `NOP` (No Operation).

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 3 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ 00. Assembly from Scratch](./00_assembly_from_scratch.md) | [Section 3 Hub](./README.md) | [02. ARM Instruction Encoding ➡️](./02_arm_instruction_encoding.md) |
