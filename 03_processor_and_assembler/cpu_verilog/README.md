# ⚡ Project 2: Building a 32-Bit ARM7 CPU Core in Verilog

---
### 🧭 Section 3 Quick Links
[🏠 Section 3 Hub](../README.md) • [01. What is an ISA?](../01_what_is_an_isa.md) • [02. ARM Instruction Encoding](../02_arm_instruction_encoding.md) • [03. Pipeline Architecture](../03_cpu_pipeline_architecture.md) • [🐍 Python Assembler](../assembler/README.md) • [⚡ ARM7 Verilog CPU](./README.md) • [🚀 BootROM](../bootrom/README.md)
---

In this project, you construct a complete **32-bit ARM7 (ARMv4T-compatible) Pipelined Processor Core** in synthesizable Verilog HDL.

---

## 🏛️ CPU Architecture Block Diagram

```
 ┌─────────────────────────────────────────────────────────────────────────────────┐
 │                                   32-BIT ARM7 CPU CORE                          │
 │                                                                                 │
 │   ┌──────────────┐          ┌──────────────┐          ┌───────────────────────┐ │
 │   │ FETCH STAGE  │          │ DECODE STAGE │          │     EXECUTE STAGE     │ │
 │   ├──────────────┤          ├──────────────┤          ├───────────────────────┤ │
 │   │              │ 32-bit   │              │ Operands │ ┌───────────────────┐ │ │
 │   │ Program      ├─────────►│ Instruction  ├─────────►│ │      32-Bit       │ │ │
 │   │ Counter (PC) │ Inst     │ Decoder      │ Rn, Rm   │ │     ALU Core      │ │ │
 │   │              │          │              │          │ └─────────┬─────────┘ │ │
 │   └──────▲───────┘          └──────▲───────┘          │           │ Result    │ │
 │          │                         │                  │           ▼           │ │
 │          │ Branch Target           │ Register Read    │ ┌───────────────────┐ │ │
 │          │ (Flush on Taken)        │ Data             │ │ CPSR Flags (NZCV) │ │ │
 │          │                         │                  │ └───────────────────┘ │ │
 │          │                  ┌──────┴───────┐          │           │           │ │
 │          │                  │ REGISTER     │◄─────────┼───────────┘ Writeback │ │
 │          │                  │ FILE (R0-R15)│  W_Data  │                       │ │
 │          │                  └──────────────┘          └───────────────────────┘ │
 └──────────┼────────────────────────────────────────────────────────┼─────────────┘
            │                                                        │
            ▼                                                        ▼
 ┌─────────────────────────────────────────────────────────────────────────────────┐
 │                   64 KB UNIFIED MEMORY & MMIO CONTROLLER (RAM & UART)           │
 └─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 🧩 Component Directory

| Verilog File | Module Name | Role & Responsibility |
| :--- | :--- | :--- |
| [`alu.v`](./alu.v) | `alu` | 32-bit Arithmetic Logic Unit (ADD, SUB, AND, ORR, XOR, Shifts, Condition Flags `NZCV`) |
| [`register_file.v`](./register_file.v) | `register_file` | 16 32-bit registers (`R0`–`R15`) with dual read ports and `PC+8` override |
| [`decoder.v`](./decoder.v) | `decoder` | Bitfield extractor (Condition, Opcode, `Rn`, `Rd`, `Rm`, Rotated Immediates) |
| [`cpu_core.v`](./cpu_core.v) | `cpu_core` | 3-Stage Pipeline (Fetch -> Decode -> Execute) with **Operand Forwarding & Branch Flushing** |
| [`memory_bus.v`](./memory_bus.v) | `memory_bus` | 64 KB RAM model + Memory-Mapped I/O (UART Data & Status registers) |
| [`cpu_top.v`](./cpu_top.v) | `cpu_top` | Top-level SoC connecting CPU Core to Memory and UART |
| [`sim_main.cpp`](./sim_main.cpp) | *(C++ Testbench)* | Verilator runner that loads `.hex` machine code, clocks the CPU, and dumps VCD traces |

---

## 🚀 How to Build and Simulate the CPU

Run the pre-configured tests using `make`:

### 1. Test 1: Arithmetic & Conditional Branch (`test_add`)
Assembles and runs [`examples/add.s`](../assembler/examples/add.s):
```bash
make test_add
```
**Expected Output:**
```text
=======================================================
 🏛️ FINAL CPU REGISTER STATE (After 100 Cycles)
=======================================================
 PC: 0x00000028
 R0: 0x0000000a (10)
 R1: 0x00000014 (20)
 R2: 0x0000001e (30)
 R3: 0x00000019 (25)
 R4: 0x00000001 (1)  <── Success branch taken!
=======================================================
```

---

### 2. Test 2: Iterative Fibonacci Loop (`test_fib`)
Assembles and runs [`examples/fibonacci.s`](../assembler/examples/fibonacci.s) (10 loop iterations):
```bash
make test_fib
```
**Expected Output:**
```text
=======================================================
 🏛️ FINAL CPU REGISTER STATE (After 100 Cycles)
=======================================================
 PC: 0x00000028
 R0: 0x00000037 (55)
 R1: 0x00000059 (89)
 R2: 0x00000059 (89) <── 10th Fibonacci number computed in hardware!
 R3: 0x00000000 (0)
=======================================================
```

---

### 3. Inspecting Waveforms in GTKWave:
Every simulation run records a `cpu_trace.vcd` file:
```bash
gtkwave cpu_trace.vcd
```

---

## 🛠️ Step-by-Step Hands-On Tasks

---

### 🟢 Task 1: Trace Data Forwarding
Look at lines in [`cpu_core.v`](./cpu_core.v#L119-L125):
```verilog
wire forward_r1 = rf_we && (rf_waddr != 4'd15) && (rf_waddr == dec_rn);
wire forward_r2 = rf_we && (rf_waddr != 4'd15) && (rf_waddr == (dec_is_mem ? dec_rd : dec_rm));
```

#### ❓ Task 1 Challenge:
> **Question:** Why does the CPU need operand forwarding when executing two back-to-back instructions like:
> ```text
> MOV R0, #10
> ADD R1, R0, #5
> ```
> *(Test your prediction, then check Answer 1 in the Answer Key at the bottom!)*

---

### 🟢 Task 2: Write Your Own Custom Assembly Test
Create a new assembly program `my_test.s` in `assembler/examples/`:
```text
    MOV R0, #2
    MOV R1, #3
    MUL_SIM:
    ADD R2, R2, R0
    SUBS R1, R1, #1
    BNE MUL_SIM
halt:
    B halt
```

Run it through the CPU:
```bash
python3 ../assembler/asm.py ../assembler/examples/my_test.s -o my_test.hex -f hex
./obj_dir/Vcpu_top my_test.hex
```

#### ❓ Task 2 Challenge:
> **Question:** What final decimal number is stored in `R2`?  
> *(Test your prediction, then check Answer 2 in the Answer Key at the bottom!)*

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why does `register_file.v` return `pc_in` (`PC + 8`) whenever the instruction reads register `R15`?

### ❓ Quiz 2:
> In `memory_bus.v`, what happens when the CPU executes `STRB R0, [0x10000000]`?

### ❓ Quiz 3:
> When a branch is taken, why are both the Fetch and Decode stages cleared with `NOP` bubbles?

---

## 🎯 Summary Checklist: Verilog CPU Design

1. **3-Stage Pipeline:** Fetch -> Decode -> Execute.
2. **Data Forwarding:** Eliminates Read-After-Write stalls for back-to-back arithmetic instructions.
3. **Branch Flushing:** Discards stale pre-fetched instructions when a jump occurs.
4. **Memory-Mapped I/O:** CPU interacts with peripherals (UART) through standard memory read/write instructions.

---

## 🔑 Answer Key & Deep Explanations

### Task Challenges:
* **Answer 1:** Because `MOV R0, #10` only writes its answer into the Register File at the end of its Execute cycle. At that exact moment, `ADD R1, R0, #5` is in the Decode stage trying to read `R0`. Without forwarding, it would read old/stale data!
* **Answer 2:** `R2 = 6` (Repeated addition multiplication: 2 + 2 + 2 = 6).

### Quizzes:
* **Answer Quiz 1:** Because in a 3-stage pipeline, the instruction being fetched from RAM is 2 instructions (8 bytes) ahead of the instruction currently executing in the ALU.
* **Answer Quiz 2:** The address decoder detects `0x10000000` (UART TX Data Register), captures the bottom byte of `R0`, and pulses `uart_tx_valid` to transmit the character out the serial wire!
* **Answer Quiz 3:** Because those 2 instructions were pre-fetched sequentially from the old code path and must be discarded so the CPU can begin executing from the branch target address.

---

## 🧭 Navigation
| ⬅️ Previous Project | 🏠 Section 3 Hub | ➡️ Next Project |
| :--- | :---: | ---: |
| [⬅️ 🐍 Project 1: Python Assembler](../assembler/README.md) | [Section 3 Hub](../README.md) | [🚀 Project 3: The BootROM ➡️](../bootrom/README.md) |
