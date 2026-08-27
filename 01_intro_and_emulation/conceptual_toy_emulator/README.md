# Conceptual Toy Emulator: From Transistor to Counter in C++

---
### 🧭 Chapter 1 Quick Links
[🏠 Section 1 Overview](../README.md) • [01. Transistors to Gates](../01_transistors_to_gates.md) • [02. FPGAs & LUTs](../02_fpgas_and_luts.md) • [03. Hardware Emulation](../03_hardware_emulation.md) • [04. Anatomy of a Chip](../04_anatomy_of_a_chip.md) • [🧪 C++ Toy Sim](./README.md) • [🔬 Verilator Lab](../lab_verilator/README.md)
---

This mini-project demonstrates the exact hierarchy of a digital computer in pure standard C++ with **zero external dependencies**.

---

## 📁 Code Architecture Overview

The toy emulator is structured into two focused files:

| File | Role | What It Contains |
| :--- | :--- | :--- |
| [`toy_emulator.hpp`](./toy_emulator.hpp) | **The Hardware Blueprint (Header)** | All transistor switches, CMOS logic gates, sequential latches, D flip-flops, arithmetic adders, and the complete 4-bit counter module. |
| [`toy_emulator.cpp`](./toy_emulator.cpp) | **The Simulation Testbench (Runner)** | The clock generator, reset driver, step-by-step cycle executor, and formatted ASCII waveform printer. |

---

## 🤔 What is This? And How is it Different from the Verilator Lab?

There are two labs in this chapter. They teach the same idea from two completely different directions:

| | `conceptual_toy_emulator/` (This folder) | `lab_verilator/` |
| :--- | :--- | :--- |
| **Language** | Pure C++ | Verilog + C++ |
| **Purpose** | *"See how hardware WORKS from first principles"* | *"See how engineers DESIGN hardware"* |
| **What you write** | A software model that behaves like physical silicon | An actual synthesizable hardware blueprint |
| **Who compiles it** | `g++` (standard C++ compiler) | Verilator (a hardware compiler) |
| **Output** | ASCII timing diagram in the terminal | `.vcd` waveform file viewed in GTKWave |

### This lab: Learning From the Bottom Up

You write C++ functions that model physical silicon switches:

```cpp
inline LogicLevel nmos_transistor(LogicLevel gate, LogicLevel source) {
    if (gate == HIGH) {
        return source; // Switch is closed: passes source (typically GND) to drain
    }
    return LOW; // Disconnected
}
```

By assembling these functions layer by layer, you experience the exact silicon pyramid:

```
Transistors (NMOS/PMOS) → CMOS NAND/NOT → SR/D Latches → D Flip-Flops → 4-Bit Counter
```

> **Analogy:** Building a model aeroplane out of cardboard.  
> It does not physically fly, but you understand every curve and structural spar.

---

## 🏛️ Inside `toy_emulator.hpp`: The 5 Hardware Levels

[`toy_emulator.hpp`](./toy_emulator.hpp) defines the complete hardware stack from ground up:

```
 [ Level 4: 4-Bit Synchronous Binary Counter ]
                 │ (Class Counter4Bit: 4 DFFs + ripple incrementer + reset logic)
                 ▼
 [ Level 3: Combinational Arithmetic ]
                 │ (struct AdderResult & half_adder: XOR Sum + AND Carry)
                 ▼
 [ Level 2: Sequential Memory ]
                 │ (SRLatchNand, DLatch, Master-Slave DFlipFlop)
                 ▼
 [ Level 1: CMOS Logic Gates ]
                 │ (cmos_not, cmos_nand, gate_and, gate_or, gate_xor)
                 ▼
 [ Level 0: Physical CMOS Transistor Switches ]
                   (nmos_transistor pulls to GND, pmos_transistor pulls to VDD)
```

### Key Components Defined in `toy_emulator.hpp`:
1. **Level 0 — `nmos_transistor` & `pmos_transistor`:** Pure switch primitives modeling conduction channels based on gate voltage.
2. **Level 1 — `cmos_not` & `cmos_nand`:** Physical pull-up/pull-down networks. Non-inverting gates (`gate_and`, `gate_or`, `gate_xor`) are derived from universal NANDs.
3. **Level 2 — `SRLatchNand` & `DFlipFlop`:** Cross-coupled NAND feedback loops trapping 1 bit of data, arranged as Master-Slave to trigger only on the rising clock edge.
4. **Level 3 — `half_adder`:** Computes single-bit sum (A XOR B) and carry (A AND B).
5. **Level 4 — `Counter4Bit`:** Connects 4 D Flip-Flops in parallel with combinational incrementers and synchronous reset masking.

---

## ⚡ Inside `toy_emulator.cpp`: The Simulation Testbench

[`toy_emulator.cpp`](./toy_emulator.cpp) acts as the virtual lab bench (power supply, signal generator, and oscilloscope screen):

1. **Instantiates the Hardware:** Creates an instance of `Counter4Bit`.
2. **Drives Signals:** Runs a simulation loop of 36 half-steps where:
   - `clk` alternates between `LOW` (0) and `HIGH` (1).
   - `reset` is held `ACTIVE` for the first 4 steps, then released.
3. **Step Clocking:** Calls `counter.step(clk, reset)` on every clock edge.
4. **Draws Waveforms:** On each rising clock edge (`clk == HIGH`), reads the 4-bit register value and prints a real-time ASCII bar graph.

---

## 🏃 How to Build and Run

Compile and run with a single command via the Makefile:
```bash
make run
```

Or manually using `g++`:
```bash
g++ -O2 -Wall -Wextra -std=c++17 -o toy_emulator toy_emulator.cpp
./toy_emulator
```

### Expected Terminal Output:
```text
==============================================================
   FROM THE TRANSISTOR TO A 4-BIT CLOCKED DIGITAL COUNTER     
   (Simulated from physical CMOS NMOS/PMOS transistor models) 
==============================================================

  Step   Clock   Reset    Binary Q   Decimal   Waveform Track
--------------------------------------------------------------
     0    HIGH  ACTIVE        0000         0   |
     1    HIGH  ACTIVE        0000         0   |
     2    HIGH       0        0001         1   |##
     3    HIGH       0        0010         2   |####
     4    HIGH       0        0011         3   |######
     5    HIGH       0        0100         4   |########
     6    HIGH       0        0101         5   |##########
     7    HIGH       0        0110         6   |############
     8    HIGH       0        0111         7   |##############
     9    HIGH       0        1000         8   |################
    10    HIGH       0        1001         9   |##################
    11    HIGH       0        1010        10   |####################
    12    HIGH       0        1011        11   |######################
    13    HIGH       0        1100        12   |########################
    14    HIGH       0        1101        13   |##########################
    15    HIGH       0        1110        14   |############################
    16    HIGH       0        1111        15   |##############################
    17    HIGH       0        0000         0   |
--------------------------------------------------------------

✅ Simulation complete! You just watched physical transistor
   switches assemble into logic gates, memory latches, and
   a fully functioning 4-bit digital computer counter!
```

---

## 🧭 Navigation
| ⬅️ Previous Guide | 🏠 Overview | ➡️ Next Lab |
| :--- | :---: | ---: |
| [⬅️ 04. Anatomy of a Microchip](../04_anatomy_of_a_chip.md) | [Section 1 Hub](../README.md) | [🔬 Verilator Lab ➡️](../lab_verilator/README.md) |
