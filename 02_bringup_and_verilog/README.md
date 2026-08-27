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

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Root Overview | ➡️ Next Section |
| :--- | :---: | ---: |
| [⬅️ Section 1: Intro & Emulation](../01_intro_and_emulation/README.md) | [Sand to Screen Hub](../README.md) | [Section 3: Processor Core ➡️](../README.md) |
