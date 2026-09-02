# 03. CPU Pipeline Architecture: The 3-Stage Engine

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](./README.md) • [01. What is an ISA?](./01_what_is_an_isa.md) • [02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) • [03. Pipeline Architecture](./03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./assembler/README.md) • [⚡ ARM7 Verilog CPU](./cpu_verilog/README.md) • [🚀 BootROM](./bootrom/README.md)
---

In early computer designs, a CPU executed instructions strictly one-by-one:
1. Wait 3 clock cycles to complete Instruction 1.
2. Wait 3 clock cycles to complete Instruction 2.
3. Total throughput: 1 instruction every 3 clock cycles.

In 1985, the Acorn Archimedes team (the creators of ARM) utilized a technique that revolutionized computing: **Instruction Pipelining**.

With pipelining, the CPU overlaps execution so that **one instruction completes on EVERY SINGLE CLOCK TICK**!

---

## 🧺 The Laundry Analogy (How Pipelining Works)

Imagine doing 4 loads of laundry (Wash -> Dry -> Fold):

```
NON-PIPELINED (Serial):
  Load 1: [ Wash ] [ Dry ] [ Fold ]
  Load 2:                          [ Wash ] [ Dry ] [ Fold ]
  Time:   1        2       3       4        5       6  (12 hours total!)

PIPELINED (Parallel Assembly Line):
  Load 1: [ Wash ] [ Dry  ] [ Fold ]
  Load 2:          [ Wash ] [ Dry  ] [ Fold ]
  Load 3:                   [ Wash ] [ Dry  ] [ Fold ]
  Load 4:                            [ Wash ] [ Dry  ] [ Fold ]
  Time:   1        2        3        4        5        6  (6 hours total!)
```

Notice that once the pipeline is full (at Time = 3), **one clean load of laundry finishes on every single hour**!

---

## 🏛️ The Classic ARM7 3-Stage Pipeline

The ARM7TDMI processor core is organized into **three distinct physical stages**:

```
                    ┌────────────────────────────────────────────────────────┐
                    │               THE 3-STAGE ARM PIPELINE                 │
                    ├──────────────────┬──────────────────┬──────────────────┤
                    │  STAGE 1: FETCH  │ STAGE 2: DECODE  │ STAGE 3: EXECUTE │
                    ├──────────────────┼──────────────────┼──────────────────┤
                    │ Reads 32-bit     │ Extracts opcode, │ ALU performs     │
                    │ instruction from │ reads register   │ math, memory     │
                    │ RAM at PC        │ ports Rn & Rm    │ load/store, or   │
                    │                  │                  │ writes back Rd   │
                    └──────────────────┴──────────────────┴──────────────────┘
```

```
 Clock Cycle 1:  [ Fetch Inst 1 ]
 Clock Cycle 2:  [ Fetch Inst 2 ] [ Decode Inst 1 ]
 Clock Cycle 3:  [ Fetch Inst 3 ] [ Decode Inst 2 ] [ Execute Inst 1 ] <── Output 1 ready!
 Clock Cycle 4:  [ Fetch Inst 4 ] [ Decode Inst 3 ] [ Execute Inst 2 ] <── Output 2 ready!
 Clock Cycle 5:  [ Fetch Inst 5 ] [ Decode Inst 4 ] [ Execute Inst 3 ] <── Output 3 ready!
```

---

## 🧠 The Famous ARM `PC = PC + 8` Quirk

Because of the 3-stage pipeline, the Program Counter (`R15 / PC`) behaves in a fascinating way that every compiler writer and chip designer must know:

When an instruction is in the **EXECUTE** stage:
* The instruction right behind it is in **DECODE** (PC + 4).
* The instruction currently being fetched from RAM is in **FETCH** (PC + 8).

```
 Address 0x00:  ADD R0, R1, R2   <── (Currently in EXECUTE stage!)
 Address 0x04:  SUB R3, R4, R5   <── (Currently in DECODE stage: PC + 4)
 Address 0x08:  MOV R6, #10      <── (Currently in FETCH stage:  PC + 8)
```

> **The Hardware Rule:** In ARM7, reading `PC` (`R15`) as an operand inside an executing instruction **always returns `PC + 8`** (the address of the instruction in the FETCH stage)!

---

## 🌪️ Pipeline Hazards & Branch Flushing

What happens when your code hits a jump instruction (like `B target` or `BL my_function`)?

When a Branch instruction reaches the **EXECUTE** stage and changes the `PC`, the two instructions that were already fetched into the pipeline (`PC + 4` and `PC + 8`) are **from the wrong branch path**!

```
 Cycle 1: [ Fetch Target ] ──► [ FLUSH (Bubble) ] ──► [ Execute Branch: JUMP! ]
 Cycle 2:                      [ Fetch Target+4 ] ──► [ FLUSH (Bubble) ]
 Cycle 3:                                             [ Execute Target! ]
```

### The Solution: Pipeline Flush (Bubbles)
The hardware automatically clears the pipeline by injecting **NOP (No Operation) bubbles**, discarding the invalid instructions, and refilling the pipeline from the new target address. 

*(A branch in ARM7 costs 3 clock cycles: 1 cycle for the branch, plus 2 cycles to refill the pipeline).*

---

## 🛠️ Step-by-Step Hands-On Exercises

---

### 🟢 Exercise 1: Pipeline Stage Tracing
Consider this sequence of 3 instructions:
```text
Address 0x100:  MOV R0, #1
Address 0x104:  ADD R1, R0, #2
Address 0x108:  SUB R2, R1, #3
```

#### ❓ Question 1:
During Clock Cycle 3, which instruction is in **FETCH**, which is in **DECODE**, and which is in **EXECUTE**?  
*(Test your prediction, then check Answer 1 at the bottom!)*

---

### 🟢 Exercise 2: The `PC + 8` Calculation
A program at address `0x00001000` executes:
```text
Address 0x00001000:   MOV R0, PC
```

#### ❓ Question 2:
What exact 32-bit value is written into register `R0`?  
*(Test your prediction, then check Answer 2 at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why does a pipelined CPU have higher throughput than a non-pipelined CPU even though both take 3 cycles to complete an individual instruction?

### ❓ Quiz 2:
> Why does taking a branch (jump) cause a 2-cycle penalty in a 3-stage pipeline?

### ❓ Quiz 3:
> In Verilog hardware, what kind of storage element is placed between pipeline stages (between Fetch and Decode, and between Decode and Execute)?

---

## 🎯 Summary Checklist: Pipelining

1. **Throughput:** Once primed, a 3-stage pipeline finishes **1 instruction per clock cycle**.
2. **The 3 Stages:** Fetch (from RAM) -> Decode (parse registers) -> Execute (ALU & Writeback).
3. **ARM PC Rule:** The Program Counter is always **8 bytes ahead** of the executing instruction.
4. **Pipeline Registers:** D Flip-Flops separate each stage to hold intermediate values on clock ticks.

---

## 🔑 Answer Key & Deep Explanations

### Exercise Challenges:
* **Answer 1:** 
  * **FETCH:** `SUB R2, R1, #3` (Address `0x108`)
  * **DECODE:** `ADD R1, R0, #2` (Address `0x104`)
  * **EXECUTE:** `MOV R0, #1` (Address `0x100`)
* **Answer 2:** `R0 = 0x00001008`. In ARM7, reading `PC` in the Execute stage yields the Fetch address (Address + 8 = `0x1000 + 8 = 0x1008`).

### Quizzes:
* **Answer Quiz 1:** Because of **parallelism**! While Instruction 1 is executing in the ALU, Instruction 2 is already being decoded, and Instruction 3 is already being fetched. All 3 hardware units work simultaneously.
* **Answer Quiz 2:** Because the next 2 instructions in the pipeline were already fetched sequentially from the old address. When the branch jumps to a new address, those 2 stale instructions must be flushed (discarded) and replaced with NOPs.
* **Answer Quiz 3:** **Pipeline Registers (D Flip-Flops)!** Wide banks of D Flip-Flops store the instruction word, register values, and control flags on every rising clock edge.

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 3 Hub | ➡️ Next Project |
| :--- | :---: | ---: |
| [⬅️ 02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) | [Section 3 Hub](./README.md) | [🐍 Project 1: Python Assembler ➡️](./assembler/README.md) |
