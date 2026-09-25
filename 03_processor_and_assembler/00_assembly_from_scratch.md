# 00. Assembly from Scratch: The Gentle Onramp

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](./README.md) • [00. Assembly from Scratch](./00_assembly_from_scratch.md) • [01. What is an ISA?](./01_what_is_an_isa.md) • [02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) • [03. Pipeline Architecture](./03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./assembler/README.md) • [⚡ ARM7 Verilog CPU](./cpu_verilog/README.md) • [🚀 BootROM](./bootrom/README.md)
---

If you have ever felt intimidated looking at code like `ADDEQ R0, R1, #4` or words like **ISA**, **CPSR**, **Operand2**, or **Pipeline Hazards**, take a deep breath.

You are not alone. Transitioning from high-level software (like Python or C) or basic digital gates into computer architecture feels like landing in a country where everyone speaks a foreign dialect made entirely of acronyms.

This guide is your **Rosetta Stone**. Before we look at official ARM manuals or 32-bit machine code bitfields, we are going to build an intuitive, zero-jargon mental model of **what assembly language actually is** and **how a processor thinks**.

---

## 🪑 The Core Mental Model: The Office Desk

Forget about silicon, electrons, and clock pulses for a moment. 

Think of a computer CPU as a **very fast, very literal person sitting at an office desk**:

```
  ┌─────────────────────────────────────────────────────────────────────────┐
  │                        YOUR OFFICE DESK (THE CPU)                       │
  │                                                                         │
  │  ┌───────────────────────────┐         ┌─────────────────────────────┐  │
  │  │      THE CALCULATOR       │         │       16 STICKY NOTES       │  │
  │  │         (The ALU)         │         │        (The Registers)      │  │
  │  │                           │◄───────►│                             │  │
  │  │  Can only ADD, SUBTRACT,  │         │  Labeled R0, R1, R2 ... R15 │  │
  │  │  AND, OR, or COMPARE      │         │  Right in front of your     │  │
  │  │  two numbers at a time!   │         │  hands. Zero delay to read! │  │
  │  └───────────────────────────┘         └─────────────────────────────┘  │
  └────────────────────────────────────┬────────────────────────────────────┘
                                       │
                                       │ Walk across the room
                                       ▼
  ┌─────────────────────────────────────────────────────────────────────────┐
  │                     THE WAREHOUSE BOOKSHELF (THE RAM)                   │
  │                                                                         │
  │  A massive wall of millions of numbered cubbies.                        │
  │  Holds all your long-term data and program instructions.                │
  │  To use something, you have to walk over, fetch it to your desk (LDR),  │
  │  or walk over and put something back on the shelf (STR).                │
  └─────────────────────────────────────────────────────────────────────────┘
```

Everything a CPU does all day is just this person repeating three actions:
1. Look at a sticky note on the desk.
2. Punch two numbers into the desktop calculator.
3. Write the answer onto another sticky note.

---

## 📖 The Jargon Buster Dictionary

Whenever you see a scary computer engineering term in this course, refer back to this translation table:

| The Technical Term | What It Actually Means | The Plain English Translation |
| :--- | :--- | :--- |
| **Register** | A tiny storage box on the CPU chip | A **sticky note on your desk** labeled with a name like `R0`, `R1`, or `R2`. |
| **Mnemonic** | A human-readable operation name | A short nickname for a command: `ADD` (Add), `SUB` (Subtract), `MOV` (Move/Copy). |
| **Immediate (`#`)** | A constant raw number | A literal number written in your code, like `#5` or `#42`, not a register. |
| **Operand** | An input to an operation | The ingredients of the math (e.g. In `x + y`, `x` and `y` are the operands). |
| **Opcode** | Operation Code | The raw binary number the hardware uses to identify the instruction. |
| **Flag** | A 1-bit indicator light | A **scoreboard light** (like a car's check engine light) that turns ON if math was zero or negative. |
| **CPSR** | Current Program Status Register | The little control panel on your desk holding the 4 scoreboard lights (`NZCV`). |
| **Branch** | An execution jump | Telling the CPU: *"Stop reading line-by-line; jump down to line 50."* |
| **Label** | A named location in code | A bookmark tag in your code (like `loop:` or `exit:`) so you can jump to it. |
| **Program Counter (PC)** | Special register `R15` | A finger pointing at the instruction currently being executed. Moves `+4` each step. |
| **ISA** | Instruction Set Architecture | The **complete restaurant menu** of every single command that specific chip understands. |

---

## 🔬 Anatomy of an Assembly Instruction

Look at this standard line of assembly:

```text
       ADD      R1,      R2,      #4
        │        │        │        │
        │        │        │        └── The Second Source (Immediate value: the number 4)
        │        │        └─────────── The First Source Register (Read from R2)
        │        └──────────────────── The Destination Register (Write result into R1)
        └───────────────────────────── The Mnemonic (The Operation: Addition)
```

In plain English, this single instruction tells the CPU:
> *"Take the number sitting on Sticky Note `R2`, add the number `4` to it, and write the final result onto Sticky Note `R1`."*

---

## 🔄 Translating High-Level Code to Assembly

Let's see how concepts you already know from Python or C look in Assembly:

### 1. Variables and Assignment
* **In Python / C:**
  ```c
  int x = 10;
  int y = 20;
  ```
* **In ARM Assembly:**
  ```armasm
  MOV R0, #10     @ Put 10 into Register R0 (x)
  MOV R1, #20     @ Put 20 into Register R1 (y)
  ```
  *(Notice: `MOV` is short for "Move", but it actually means "Copy" or "Set").*

---

### 2. Basic Math
* **In Python / C:**
  ```c
  int z = x + y;
  ```
* **In ARM Assembly:**
  ```armasm
  ADD R2, R0, R1  @ R2 = R0 + R1 (z = x + y)
  ```

---

### 3. Decisions & "If" Statements: The Scoreboard (Flags)
How does a computer make a decision like `if (x == y)`?

A CPU cannot "think". It can only subtract!

When you tell the CPU to compare two numbers (`CMP R0, R1`), the CPU secretly **subtracts them** (`R0 - R1`) and looks at what happened:

```
  If R0 == R1:
      R0 - R1 = 0
      The result is ZERO!
      The CPU turns ON the "Z" (Zero) scoreboard light!
```

Once the **`Z` (Zero) light** is ON, other instructions can check that light:

* **In Python / C:**
  ```c
  if (x == y) {
      z = 1;
  } else {
      z = 0;
  }
  ```
* **In Traditional Assembly (Using a Branch / Jump):**
  ```armasm
      CMP R0, R1          @ Subtract R0 - R1. If they are equal, turn on Z flag!
      BNE not_equal       @ "Branch if Not Equal" (if Z is OFF, jump away!)
      MOV R2, #1          @ Equal! Set z = 1
      B finish            @ Jump to finish

  not_equal:
      MOV R2, #0          @ Not equal! Set z = 0

  finish:
      @ continue...
  ```

---

## ⚡ Demystifying ARM's Superpower: Conditional Execution

Now you are ready to understand the piece of code that looked alien earlier!

Look at what we just wrote above: we had to make the CPU **jump back and forth** over lines of code just to set `R2` to `1` or `0`.

Jumping is slow because the CPU was already loading the next instruction into its pipeline and now has to throw it away.

ARM has a genius feature called **Conditional Execution**. Instead of jumping, you write the condition directly onto the instruction name:

| Suffix | Condition Name | When Does It Run? |
| :---: | :--- | :--- |
| **`EQ`** | **Equal** | Runs only if the Zero flag is **ON** (`Z == 1`). |
| **`NE`** | **Not Equal** | Runs only if the Zero flag is **OFF** (`Z == 0`). |
| **`LT`** | **Less Than** | Runs only if the Negative and Overflow flags differ (`N != V`). |
| **`GT`** | **Greater Than**| Runs only if positive and not zero. |

Now look at how clean the exact same `if/else` becomes in ARM:

```armasm
    CMP   R0, R1       @ Compare R0 and R1 (sets the scoreboard flags)
    MOVEQ R2, #1       @ If Equal, set R2 = 1. (If not equal, hardware skips this line!)
    MOVNE R2, #0       @ If Not Equal, set R2 = 0. (If equal, hardware skips this line!)
```

**Look at what happened:**
* No labels!
* No jumping around (`B` / `BNE`)!
* Zero wasted pipeline cycles!
* If the condition is false, the hardware simply turns that single instruction into a harmless, 1-cycle **`NOP` (No Operation)** and keeps flowing smoothly!

---

## 📦 What About RAM? (Memory Access)

Why can't we just write `ADD R0, R1, [0x1000]` to add a number directly from RAM?

Because the CPU's calculator (ALU) **can only reach the sticky notes on its desk (Registers)**. The RAM bookshelf is too far away across the room.

To use data from RAM, you follow a strict 2-step procedure:

1. **`LDR` (Load Register):** Walk over to the bookshelf, take the number from address `[R4]`, and bring it back to sticky note `R0`.
   ```armasm
   LDR R0, [R4]        @ R0 = Memory[R4]
   ```
2. **`STR` (Store Register):** Take the answer from sticky note `R0`, walk over to the bookshelf, and save it into address `[R4]`.
   ```armasm
   STR R0, [R4]        @ Memory[R4] = R0
   ```

This architecture is called a **Load-Store Architecture**, and it is the hallmark of modern RISC chips (ARM, Apple Silicon M-series, RISC-V).

---

## 🛠️ Hands-On Practice Exercises

Trace these simple code snippets in your head to build your intuition.

---

### 🟢 Exercise 1: Register Math
What are the contents of `R0`, `R1`, and `R2` after these three lines execute?

```armasm
MOV R0, #15
MOV R1, #5
SUB R2, R0, R1
```

#### ❓ Question 1:
> What is the final value stored in `R2`?  
> *(Test your prediction, then check Answer 1 in the Answer Key at the bottom!)*

---

### 🟢 Exercise 2: Conditional Skip
Look at this code:

```armasm
MOV R0, #7
MOV R1, #7
CMP R0, R1
ADDEQ R2, R0, #3
SUBNE R2, R0, #3
```

#### ❓ Question 2:
> Does `ADDEQ` execute, or does `SUBNE` execute? What is the final value of `R2`?  
> *(Test your prediction, then check Answer 2 in the Answer Key at the bottom!)*

---

### 🟢 Exercise 3: Reading Memory
Suppose RAM address `0x1000` contains the number `42`.
The CPU executes:

```armasm
MOV R4, #0x1000
LDR R0, [R4]
ADD R0, R0, #8
```

#### ❓ Question 3:
> What is the final value of `R0`?  
> *(Test your prediction, then check Answer 3 in the Answer Key at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why does assembly language use registers (`R0`–`R15`) instead of just reading and writing memory (RAM) directly on every line of code?

### ❓ Quiz 2:
> In the instruction `ADD R0, R1, #10`, what does the `#` symbol indicate to the assembler?

### ❓ Quiz 3:
> What does the instruction `CMP R0, R1` actually do physically inside the ALU?

---

## 🎯 Summary Checklist: Thinking in Assembly

1. **Registers are Sticky Notes:** The CPU only does math on its 16 internal registers (`R0`–`R15`).
2. **ALU is the Calculator:** It performs additions, subtractions, and bitwise logic.
3. **Decisions are Subtractions:** `CMP` subtracts two values and turns on scoreboard lights (Flags: `N`, `Z`, `C`, `V`).
4. **Conditional Execution is a Filter:** Suffixes like `EQ` or `NE` let instructions automatically execute or turn into a harmless NOP.
5. **RAM Requires LDR and STR:** You cannot do math directly in memory; you must load into registers first!

---

## 🔑 Answer Key & Deep Explanations

### Exercise Challenges:
* **Answer 1:** `R2 = 10`. `15 - 5 = 10`.
* **Answer 2:** `ADDEQ` executes! Because `R0` and `R1` are both `7`, `CMP R0, R1` finds they are equal (7 - 7 = 0) and turns ON the Zero flag (`Z = 1`). `ADDEQ` sees `Z == 1` and runs (7 + 3 = 10). `SUBNE` sees `Z == 1`, fails its condition, and is ignored as a NOP. `R2 = 10`.
* **Answer 3:** `R0 = 50`. `LDR` brings the value `42` from memory into `R0`. Then `ADD` adds `8` (42 + 8 = 50).

### Quizzes:
* **Answer Quiz 1:** Speed! Registers sit directly on the CPU silicon die right next to the ALU and can be read in a fraction of a nanosecond. Reading RAM requires driving external buses across the chip or motherboard, which is dozens to hundreds of times slower.
* **Answer Quiz 2:** It indicates an **Immediate value** (a constant literal number) rather than a register index. `ADD R0, R1, 10` would be confusing without `#` because the assembler needs to know you mean the number 10, not Register 10!
* **Answer Quiz 3:** It physically performs the subtraction `R0 - R1` inside the adder circuit, updates the condition flags (`N`, `Z`, `C`, `V`) based on the result, and then **discards the arithmetic result without modifying any registers**.

---

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Section 3 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ Section 2 Hub](../02_bringup_and_verilog/README.md) | [Section 3 Hub](./README.md) | [01. What is an ISA? ➡️](./01_what_is_an_isa.md) |
