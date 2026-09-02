# Section 3: Processor — What is a Processor Anyway?

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](./README.md) • [01. What is an ISA?](./01_what_is_an_isa.md) • [02. ARM Instruction Encoding](./02_arm_instruction_encoding.md) • [03. Pipeline Architecture](./03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](./assembler/README.md) • [⚡ ARM7 Verilog CPU](./cpu_verilog/README.md) • [🚀 BootROM](./bootrom/README.md)
---

Welcome to Section 3 of **From the Transistor to the Web Browser**!

In Section 1 and Section 2, we built fixed-function circuits: adders, counters, and serial state machines. But a fixed-function chip can only do **one thing**.

A **Processor (CPU)** is fundamentally different: it is a **universal programmable state machine** that reads arbitrary instructions from memory and executes them sequentially.

In this section, you will design and build a **32-bit ARM7 (ARMv4T-compatible) CPU Core in Verilog**, write an **ARM Assembler in Python**, and create a **BootROM** that boots code over UART.

---

## 🎯 Learning Goals for Section 3

By the end of this section, you will master:
1. **Instruction Set Architecture (ISA):** What the contract between software and hardware actually looks like.
2. **ARM 32-Bit Register File:** The 16 core registers (`R0`–`R15`, including Stack Pointer `R13 / SP`, Link Register `R14 / LR`, and Program Counter `R15 / PC`).
3. **Instruction Encoding & Decoding:** How human-readable assembly lines like `ADD R1, R2, #4` are packed into raw 32-bit binary integers.
4. **3-Stage Pipelining:** How the CPU overlaps **Fetch**, **Decode**, and **Execute** cycles so that one instruction completes every single clock tick.
5. **Memory-Mapped I/O (MMIO):** How a CPU communicates with peripherals (like our Section 2 UART controller) using standard load/store memory instructions.
6. **Writing an Assembler in Python:** Building a real compiler frontend that parses text assembly and emits binary machine code.
7. **The BootROM:** Writing the first 40 lines of code executed when power turns on to download software over serial into RAM.

---

## 🗺️ Section 3 Curriculum Roadmap

| # | Guide / Project | Topic | Key Concepts |
| :--- | :--- | :--- | :--- |
| **01** | [**What is an ISA?**](./01_what_is_an_isa.md) | The Hardware-Software Contract | RISC vs CISC, ARMv4T Architecture, Register File (`R0`–`R15`), CPSR Flags (`NZCV`) |
| **02** | [**ARM Instruction Encoding**](./02_arm_instruction_encoding.md) | 32-Bit Machine Code | Condition codes, Data Processing, Immediate values, Load/Store (`LDR`/`STR`), Branches (`B`/`BL`) |
| **03** | [**CPU Pipeline Architecture**](./03_cpu_pipeline_architecture.md) | Pipelining & Execution | 3-Stage Pipeline (Fetch -> Decode -> Execute), Hazards, PC offset (PC + 8), Branch flushing |
| **🐍** | [**Project 1: Python Assembler**](./assembler/README.md) | Software Toolchain | Lexing, Symbol Table (Labels), Two-pass Assembly, Binary Emission (`asm.py`) |
| **⚡** | [**Project 2: ARM7 CPU in Verilog**](./cpu_verilog/README.md) | Physical Hardware Core | ALU, 16-port Register File, Decoder, 3-stage CPU Core, RAM bus, Verilator testbench |
| **🚀** | [**Project 3: The BootROM**](./bootrom/README.md) | Hardware Bringup | Bare-metal assembly bootloader, UART serial download loop, Jumping execution to RAM |

---

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Root Overview | ➡️ Next Section |
| :--- | :---: | ---: |
| [⬅️ Section 2: Bringup & Verilog](../02_bringup_and_verilog/README.md) | [Sand to Screen Hub](../README.md) | [Section 4: Compiler (A High Level Language) ➡️](../README.md) |
