# Section 1: Intro — Cheating Our Way Past the Transistor

Welcome to **Section 1** of the *From the Transistor to the Web Browser* series.

In this section, we demystify how physical matter (silicon, voltage, electrons) transforms into programmable logic (FPGAs, CPUs), and how we can use **software emulation (Verilator)** to build real digital hardware directly on your laptop without buying expensive physical chips.

---

## 🗺️ Learning Roadmap

This folder is organized into step-by-step conceptual guides and hands-on labs:

| File / Folder | Topic | What You Will Learn |
| :--- | :--- | :--- |
| [`01_transistors_to_gates.md`](./01_transistors_to_gates.md) | **Transistors to Gates** | How MOSFETs act as voltage-controlled switches, NMOS vs PMOS, CMOS NOT/NAND/NOR gates, and the digital voltage abstraction. |
| [`02_fpgas_and_luts.md`](./02_fpgas_and_luts.md) | **FPGAs & Look-Up Tables** | How we can reconfigure silicon on the fly. What a LUT (Look-Up Table) really is, Flip-Flops, CLBs, and real-world FPGA applications. |
| [`03_hardware_emulation.md`](./03_hardware_emulation.md) | **Hardware Emulation & Verilator** | Why HDLs are not normal code, how cycle-accurate simulation works, and how Verilator translates Verilog circuits into fast C++ models. |
| [`conceptual_toy_emulator/`](./conceptual_toy_emulator/) | **Pure C++ Transistor Sim** | A zero-dependency C++ simulator that builds a 4-bit binary counter starting strictly from software-modeled transistors and NAND gates. Run it right now with `g++`! |
| [`lab_verilator/`](./lab_verilator/) | **Verilator Hands-on Lab** | Your first real Verilog module, C++ testbench, and `.vcd` waveform generation. |

---

## 🎯 Section 1 Goals

By the end of this section, you should be able to:
1. Explain how a transistor creates a binary switch using voltage.
2. Construct NOT and NAND gates from NMOS and PMOS transistors.
3. Understand how an FPGA uses SRAM bits and multiplexers to simulate any truth table without rewiring physical circuits.
4. Distinguish between sequential software execution (CPUs) and concurrent hardware execution (HDLs).
5. Compile and simulate digital logic circuits cycle-by-cycle on your laptop.

---

## 🚀 Getting Started

Start by reading:
👉 **[`01_transistors_to_gates.md`](./01_transistors_to_gates.md)**
