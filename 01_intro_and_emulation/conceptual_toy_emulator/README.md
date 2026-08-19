# Conceptual Toy Emulator: From Transistor to Counter in C++

---
### 🧭 Chapter 1 Quick Links
[🏠 Section 1 Overview](../README.md) • [01. Transistors to Gates](../01_transistors_to_gates.md) • [02. FPGAs & LUTs](../02_fpgas_and_luts.md) • [03. Hardware Emulation](../03_hardware_emulation.md) • [04. Anatomy of a Chip](../04_anatomy_of_a_chip.md) • [🧪 C++ Toy Sim](./README.md) • [🔬 Verilator Lab](../lab_verilator/README.md)
---

This mini-project demonstrates the exact hierarchy of a digital computer inside a single, clean C++ file with **no external dependencies**.

---

## 🤔 What is This? And How is it Different from the Verilator Lab?

There are two labs in this chapter. They teach the same idea from two completely different directions:

| | `conceptual_toy_emulator/` (This folder) | `lab_verilator/` |
| :--- | :--- | :--- |
| **Language** | Pure C++ | Verilog + C++ |
| **Purpose** | *"See how hardware WORKS from first principles"* | *"See how engineers DESIGN hardware"* |
| **What you write** | A software program that pretends to be transistors | An actual hardware blueprint |
| **Who compiles it** | `g++` (normal C++ compiler) | Verilator (a hardware compiler) |
| **Output** | Text printed to the terminal | A `.vcd` waveform file you view in GTKWave |

### This lab: Learning From the Bottom Up

This is **pure fiction** — a storytelling tool.

You write C++ functions that **pretend** to be transistors:

```cpp
bool nmos_transistor(bool gate, bool source) {
    return gate ? source : false;  // If gate is ON, let electricity through
}
```

There are **no actual transistors** here. It is just if/else logic inside software.  
But by building the exact same hierarchy that real silicon uses:

```
nmos() / pmos()  →  cmos_nand()  →  SRLatch()  →  DFlipFlop()  →  Counter4Bit()
```

...you feel and understand the architecture from the inside — **without needing Verilator, GTKWave, or any special tools**. Just `g++`, which you already have.

> **Analogy:** Building a model aeroplane out of cardboard.  
> It does not actually fly, but you understand exactly how a real one is shaped.

**Run this one first.** Then go to [`lab_verilator/`](../lab_verilator/README.md) to see how professional engineers describe and simulate the same hardware.

---

## 🏛️ The Hierarchy Inside `toy_emulator.cpp`

```
 [ Level 4: 4-Bit Binary Counter ]
                 │ (Uses 4 D Flip-Flops & Half Adders)
                 ▼
 [ Level 3: Master-Slave D Flip-Flops & Adders ]
                 │ (Uses Latches and Universal Gates)
                 ▼
 [ Level 2: SR-Latches & Universal Gates (AND / OR / XOR) ]
                 │ (Uses CMOS NAND and NOT gates)
                 ▼
 [ Level 1: CMOS Logic Gates (NAND / NOT) ]
                 │ (Uses NMOS & PMOS Transistor switches)
                 ▼
 [ Level 0: Physical Transistor Switches (NMOS / PMOS) ]
```

---

## 🏃 How to Run

Compile and run with one command:
```bash
make run
```

Or manually:
```bash
g++ -O2 toy_emulator.cpp -o toy_emulator
./toy_emulator
```

---

## 🔍 Things to Notice in the Code:

1. **`nmos_transistor` & `pmos_transistor`**: These are simple conditional functions simulating physical conduction channels when voltage is applied to the gate.
2. **`cmos_nand`**: Implements parallel PMOS pull-up and series NMOS pull-down.
3. **`SRLatchNand`**: Demonstrates the feedback loop of cross-coupled gates creating 1-bit of persistent memory.
4. **`Counter4Bit`**: Shows how synchronous clocking advances state reliably without race conditions.

---

## 🧭 Navigation
| ⬅️ Previous Guide | 🏠 Overview | ➡️ Next Lab |
| :--- | :---: | ---: |
| [⬅️ 04. Anatomy of a Microchip](../04_anatomy_of_a_chip.md) | [Section 1 Hub](../README.md) | [🔬 Verilator Lab ➡️](../lab_verilator/README.md) |

