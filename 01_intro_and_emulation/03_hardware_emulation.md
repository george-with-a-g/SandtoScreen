# 03. Hardware Emulation: Simulating Silicon on Your Laptop

---
### 🧭 Chapter 1 Quick Links
[🏠 Section 1 Overview](./README.md) • [01. Transistors to Gates](./01_transistors_to_gates.md) • [02. FPGAs & LUTs](./02_fpgas_and_luts.md) • [03. Hardware Emulation](./03_hardware_emulation.md) • [04. Anatomy of a Chip](./04_anatomy_of_a_chip.md) • [🧪 C++ Toy Sim](./conceptual_toy_emulator/README.md) • [🔬 Verilator Lab](./lab_verilator/README.md)
---

> **Goal:** Understand how we test and run digital hardware designs on a normal laptop without buying physical chips.

---

## 📖 Jargon Buster (Read this first!)

| Jargon Term | What it actually means in plain English |
| :--- | :--- |
| **HDL** | *(Hardware Description Language)* A programming language used to describe physical wires and circuits (like Verilog or VHDL). |
| **Verilog** | The specific hardware language we use in this course. It looks a bit like C, but describes physical hardware blocks. |
| **Emulation / Simulation** | Running a virtual model of a hardware chip inside software on your laptop. |
| **Verilator** | A fast, free tool that converts Verilog hardware code into C++ code so your laptop can simulate it at high speed. |
| **Testbench** | A helper program (written in C++ or Verilog) that acts as the "virtual lab equipment" — sending clock pulses and test signals to your circuit. |
| **VCD File** | *(Value Change Dump)* A log file that records every voltage flip on every wire over time. |
| **GTKWave** | A visual software tool that opens a VCD file and lets you view the digital waveforms like an oscilloscope screen. |

---

## 1. The Key Mindset Shift: Software vs. Hardware

Before writing hardware code, you have to change how you imagine code executing:

```
        SOFTWARE (Sequential)                       HARDWARE (Parallel)
 
      Line 1: x = a + b;                     [ Circuit 1: x = a + b ]
              │                                         │
              ▼ (Wait for line 1...)                    │ (Both operate at the
      Line 2: y = c * d;                     [ Circuit 2: y = c * d ]  same microsecond!)
              │                                         │
              ▼                                         ▼
      (One instruction at a time)            (All physical circuits exist and run together)
```

### The Factory Analogy:
* **Software:** A **single master chef** reading a recipe step by step (Step 1: Chop onion -> Step 2: Heat pan -> Step 3: Fry).
* **Hardware:** A **factory assembly line** with 100 workers. Worker A is always chopping, Worker B is always stirring, and conveyor belts connect them simultaneously.

In Verilog, when you write code, you are not writing a list of instructions. You are **connecting physical wires between virtual components**.

---

## 2. Why Emulate Hardware on a Laptop?

Why not jump straight to building real physical chips?

1. **Instant Feedback:** Compiling Verilog in software takes **2 seconds**. Flashing and synthesizing physical chips can take 30 minutes.
2. **X-Ray Vision (Debugging):** In real silicon, you cannot easily attach a probe to a microscopic wire inside the chip. In a software simulator, you can inspect **every single wire and register** at every nanosecond.
3. **Zero Cost:** Anyone with a laptop can build a full CPU and operating system for free.

---

## 3. How Verilator Works (The Bridge to C++)

Traditional hardware simulators interpret Verilog code line-by-line during testing, which is very slow.

**Verilator does something much smarter:**

```
 [ Your Verilog Blueprint (.v) ]
                │
                ▼  (Verilator translates wires into C++ math)
 [ C++ Simulation Classes (.cpp / .h) ]
                │
                ▼  (Standard g++ / clang compiler)
 [ Native Executable Binary on your Laptop ]
```

Verilator reads your Verilog circuit description and writes a **C++ class** where:
- Every wire and register becomes a C++ variable (e.g. `uint8_t count`).
- Every clock tick evaluates the boolean math in pure C++.

---

## 4. The Virtual Test Lab: The C++ Testbench

To test a circuit in real life, an engineer places a chip on a lab bench and connects:
1. A **Signal Generator** (to send clock pulses).
2. A **Power Supply / Buttons** (to press reset or enable).
3. An **Oscilloscope** (to observe the output waves).

In Verilator, your C++ testbench (`sim_main.cpp`) acts as this entire virtual lab:

```cpp
// 1. Create the virtual chip
Vcounter* top = new Vcounter;

// 2. Loop time forward (100 nanoseconds)
for (int time = 0; time < 100; time++) {
    // Flip clock pin: 0 -> 1 -> 0 -> 1
    top->clk = (time % 2 == 1);

    // Release reset button after 10 ns
    top->reset = (time < 10) ? 1 : 0;

    // Evaluate the circuit!
    top->eval();

    // Print what the chip is outputting
    std::cout << "Time: " << time << " Count: " << (int)top->count << "\n";
}
```

---

## 5. Visualizing the Signals with GTKWave

When simulation runs, Verilator writes a file called `counter.vcd`.
When you open it in **GTKWave**, you see visual wave lines showing your circuit in action:

```
Time:        0ns   10ns   20ns   30ns   40ns   50ns   60ns
             │     │      │      │      │      │      │
clk:         ┌┐    ┌┐     ┌┐     ┌┐     ┌┐     ┌┐     ┌┐
             └┴────┴┴─────┴┴─────┴┴─────┴┴─────┴┴─────┴┴─
reset:       ──────┐
                   └──────────────────────────────────────
count[3:0]:  0     │ 0    │ 1    │ 2    │ 3    │ 4    │ 5
```

---

## 🚀 Now You Are Ready!

You have all the foundational concepts:
1. **Transistors** are voltage-controlled switches.
2. **Logic Gates** make decisions with these switches.
3. **FPGAs & LUTs** store truth tables in memory to mimic any circuit.
4. **Verilator** runs and tests these circuits on your laptop.

---

## 🧭 Navigation
| ⬅️ Previous Guide | 🏠 Overview | ➡️ Next Guide |
| :--- | :---: | ---: |
| [⬅️ 02. FPGAs & LUTs](./02_fpgas_and_luts.md) | [Section 1 Hub](./README.md) | [04. Anatomy of a Microchip ➡️](./04_anatomy_of_a_chip.md) |

