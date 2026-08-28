# Section 2: Bringup — What Language is Hardware Coded In?

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](./README.md) • [01. Thinking in Verilog](./01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](./02_blocking_vs_nonblocking.md) • [03. FSM Design](./03_fsm_design.md) • [💡 Lab 1: Blinky](./lab_blinky/README.md) • [📡 Lab 2: UART](./lab_uart/README.md)
---

Welcome to Section 2 of **From the Transistor to the Web Browser**!

In Section 1, you learned how physical transistors form logic gates, adders, multiplexers, and flip-flops. In this section, we transition to **how professional hardware engineers describe, simulate, and control silicon chips using Verilog HDL**.

---

## 🎯 Learning Goals for Section 2

By the end of this section, you will understand:
1. **Parallelism vs. Sequentiality:** Why hardware description languages (HDLs) are NOT like C or Python.
2. **Wires vs. Registers (`wire` vs `reg`):** How continuous electrical nets differ from clocked storage elements.
3. **The Golden Rule of Assignments:** Exactly when and why to use Blocking (`=`) vs. Non-Blocking (`<=`) assignments to avoid dangerous race conditions.
4. **Clock Division:** How to count clock cycles to slow down a blazing fast 50 MHz oscillator to a human-visible 1 Hz LED blink.
5. **Finite State Machines (FSMs):** How digital controllers step through states (IDLE, START, DATA, STOP).
6. **Serial Communication (UART):** How to transmit and receive raw bytes over a single physical wire and interface hardware with a computer terminal.

---

## 🗺️ Section 2 Curriculum Roadmap

| # | Guide / Lab | Topic | Key Concepts |
| :--- | :--- | :--- | :--- |
| **01** | [**Thinking in Verilog**](./01_what_is_verilog.md) | The Mental Shift | Modules, Ports, `wire` vs `reg`, `assign` vs `always` |
| **02** | [**Blocking vs Non-Blocking**](./02_blocking_vs_nonblocking.md) | Avoiding Race Conditions | `=` (Combinational) vs `<=` (Clocked Sequential Flip-Flops) |
| **03** | [**Finite State Machines (FSMs)**](./03_fsm_design.md) | Hardware Brains | State encoding, Next-State logic, Output logic, 3-block FSM style |
| **💡** | [**Lab 1: Blinky LED**](./lab_blinky/README.md) | Clock Division & Simulation | 50 MHz to 1 Hz Prescaler, Verilator C++ testbench, GTKWave trace |
| **📡** | [**Lab 2: UART Controller**](./lab_uart/README.md) | Serial Transmission & MMIO | Baud generator (115200), `uart_tx.v`, `uart_rx.v`, Echo loopback |

---

## ⚡ Master Verilog Keywords & Syntax Reference

Keep this quick reference open while writing your Verilog labs:

### 1. Chip Architecture & Structure
| Keyword | Category | What It Does in Silicon | Example |
| :--- | :--- | :--- | :--- |
| **`module`** | Packaging | Starts a chip blueprint and declares port pins | `module my_chip (input wire clk, output reg q);` |
| **`endmodule`**| Packaging | Closes the chip blueprint | `endmodule` |
| **`input`** | Ports | Pin where electricity flows **INTO** the chip | `input wire [7:0] data_in` |
| **`output`** | Ports | Pin where electricity flows **OUT OF** the chip | `output reg [3:0] count` |
| **`inout`** | Ports | Bidirectional pin (both input and output) | `inout wire sda` (I2C data line) |

### 2. Data Types & Constants
| Keyword | Category | What It Does in Silicon | Example |
| :--- | :--- | :--- | :--- |
| **`wire`** | Net | Physical copper trace connecting points (no memory) | `wire [3:0] sum;` |
| **`reg`** | Driver | Target of procedural assignments (becomes Flip-Flop if clocked) | `reg [3:0] state;` |
| **`parameter`** | Constant | Module-level constant customizable at instantiation | `parameter CLK_FREQ = 50_000_000;` |
| **`localparam`**| Constant | Internal module-only constant (e.g. FSM state names) | `localparam STATE_IDLE = 2'b00;` |

### 3. Logic & Execution Blocks
| Keyword | Category | What It Does in Silicon | Example |
| :--- | :--- | :--- | :--- |
| **`assign`** | Combinational | Continuous electrical wiring (instant logic) | `assign y = a & b;` |
| **`always`** | Procedural | Block triggered by sensitivity list (`@`) | `always @(posedge clk)` or `always @(*)` |
| **`posedge`** | Clocking | Triggers on **rising edge (0V -> 5V)** | `always @(posedge clk)` (creates Flip-Flops) |
| **`negedge`** | Clocking | Triggers on **falling edge (5V -> 0V)** | `always @(negedge reset_n)` |
| **`begin` / `end`** | Grouping | Verilog's curly braces `{ ... }` | `if (en) begin q <= d; end` |

### 4. Control & Decisions
| Keyword | Category | What It Does in Silicon | Example |
| :--- | :--- | :--- | :--- |
| **`if` / `else`** | Selection | Synthesizes a priority Multiplexer | `if (sel) y = b; else y = a;` |
| **`case` / `endcase`** | Selection | Synthesizes a clean parallel Multiplexer | `case (state) ... endcase` |
| **`default`** | Safety | Fallback path in case statement (prevents latches) | `default: next_state = STATE_IDLE;` |

### 5. Assignment Operators
| Operator | Name | Where to Use | Hardware Synthesized |
| :---: | :--- | :--- | :--- |
| **`=`** | **Blocking** | Inside `always @(*)` and `assign` | Pure combinational logic gates & MUXes |
| **`<=`** | **Non-Blocking** | Inside `always @(posedge clk)` | Clocked parallel D Flip-Flops (Registers) |

---

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Root Overview | ➡️ Next Section |
| :--- | :---: | ---: |
| [⬅️ Section 1: Intro & Emulation](../01_intro_and_emulation/README.md) | [Sand to Screen Hub](../README.md) | [Section 3: Processor Core ➡️](../README.md) |
