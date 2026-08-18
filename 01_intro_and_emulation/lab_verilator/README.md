# Lab: Your First Verilator Hardware Emulation

---
### 🧭 Chapter 1 Quick Links
[🏠 Section 1 Overview](../README.md) • [01. Transistors to Gates](../01_transistors_to_gates.md) • [02. FPGAs & LUTs](../02_fpgas_and_luts.md) • [03. Hardware Emulation](../03_hardware_emulation.md) • [04. Anatomy of a Chip](../04_anatomy_of_a_chip.md) • [🧪 C++ Toy Sim](../conceptual_toy_emulator/README.md) • [🔬 Verilator Lab](./README.md)
---

## 📁 Lab Files Overview

| File | What it is |
| :--- | :--- |
| [`counter.v`](./counter.v) | The **Hardware Blueprint**: A 4-bit synchronous binary counter written in Verilog. |
| [`sim_main.cpp`](./sim_main.cpp) | The **Virtual Testbench**: A C++ program acting as the signal generator, power supply, and oscilloscope. |
| [`Makefile`](./Makefile) | Build script automating the Verilator compilation and GTKWave launching. |

---

## 🔍 Understanding the Existing Code Line-by-Line

Before modifying anything, let's understand how [`counter.v`](./counter.v) and [`sim_main.cpp`](./sim_main.cpp) work.

### 1. Dissecting `counter.v` (The Hardware)

> **Crucial Mindset Shift:** In software (C/Python), code is a list of sequential instructions. In Verilog, code is a **physical wiring diagram** for a microchip. Every line creates physical wires, switches, or memory latches.

#### A. Visualizing the Chip from the Outside (The Physical Pinout)
When you declare `module counter (...)`, imagine holding a physical black integrated circuit (IC) chip in your hand:

```
                  ┌──────────────────────┐
                  │   COUNTER CHIP       │
                  │                      │
   Clock Input ──►│ clk         count[3] ├─-► Output Wire 3 (Eight's place: 8)
   Reset Input ──►│ reset       count[2] ├─-► Output Wire 2 (Four's place: 4)
  Enable Input ──►│ enable      count[1] ├─-► Output Wire 1 (Two's place: 2)
                  │             count[0] ├─-► Output Wire 0 (One's place: 1)
                  │                      │
                  └──────────────────────┘
```
- **3 Input Pins (Left):** Incoming electrical wires carrying voltage from the outside world.
- **4 Output Pins (Right):** A bundle of 4 wires carrying voltage representing numbers from `0000` (0) up to `1111` (15).

---

#### B. The Verilog Code
```verilog
`timescale 1ns / 1ps

module counter (
    input  wire       clk,     // 1-bit Clock input
    input  wire       reset,   // 1-bit Synchronous Reset
    input  wire       enable,  // 1-bit Count Enable
    output reg  [3:0] count    // 4-bit Register Output (Pins 3, 2, 1, 0)
);

    // This block creates 4 clocked D Flip-Flops in silicon!
    always @(posedge clk) begin
        if (reset) begin
            count <= 4'b0000;         // Reset all 4 bits to 0
        end else if (enable) begin
            count <= count + 4'b0001; // Increment by 1
        end
    end

endmodule
```

---

#### C. Line-by-Line Breakdown

1. **`` `timescale 1ns / 1ps ``**
   - Sets the simulation ruler. `1ns` is the base time unit (each step `#1` is 1 nanosecond). `1ps` is the precision/rounding limit (1 picosecond).

2. **`module counter ( ... );`**
   - Declares the boundary and external pins of the hardware block.

3. **`input wire clk, reset, enable;`**
   - `wire`: A physical incoming conductor wire that passes voltage straight into the chip. Wires cannot store memory on their own.

4. **`output reg [3:0] count;`**
   - `[3:0]` defines a 4-bit bus (4 physical wires: `count[3]`, `count[2]`, `count[1]`, `count[0]`).
   - `reg` (register) tells the synthesizer: *"These output wires are attached to internal memory storage (Flip-Flops) that remember their state."*

5. **`always @(posedge clk) begin`**
   - **This is NOT a while-loop!** It does not spin continuously.
   - It tells the silicon compiler: *"Create 4 physical D Flip-Flops whose clock pins are tied to `clk`. Only unlock the Flip-Flops and evaluate new values on the rising edge (when `clk` voltage jumps from 0V to 5V)."*
   - Between clock ticks, this entire block is asleep and holds its previous value.

6. **`if (reset) count <= 4'b0000;`**
   - If the `reset` wire is HIGH (1), force all 4 Flip-Flops to zero.
   - `4'b0000` is Verilog notation: `4` bits wide, `b` for binary, value `0000`.

7. **`else if (enable) count <= count + 4'b0001;`**
   - If reset is 0 and enable is 1, pass the current 4-bit output through an adder circuit to compute `count + 1`, and latch that new value on the clock tick.

8. **`<=` (Non-Blocking Assignment):**
   - In C/Python, `=` means do this immediately right now.
   - In Verilog, `<=` tells the hardware: *"Schedule all 4 Flip-Flops to update simultaneously in parallel when the clock edge hits."*

---

#### D. What is Physically Inside the Chip?

When Verilator or an FPGA synthesizer compiles `counter.v`, it builds this exact internal circuit:

```
                    ┌────────────────────────────┐
                    │     4 D FLIP-FLOPS         │
     ┌─────────────►│ D3                      Q3 ├──┬────────► count[3]
     │              │ D2                      Q2 ├──┼────────► count[2]
     │              │ D1                      Q1 ├──┼────────► count[1]
     │              │ D0                      Q0 ├──┼────────► count[0]
     │              │   Clock Pin (clk) >        │  │
     │              └────────────────────────────┘  │
     │                            ▲                 │
     │                            │ (Clock Wire)    │
     │                                              │
     │     ┌──────────────────┐                     │
     └─────┤ 4-Bit Multiplexer│                     │
           │ (Handles Reset   │                     │
           │  & Enable rules) │                     │
           └────────▲─────────┘                     │
                    │                               │
                    └───────[ 4-Bit Adder (+1) ]◄───┘
```

1. **4 D Flip-Flops:** Store the current 4-bit count (0 to 15).
2. **4-Bit Adder:** Feeds off the current output wires (Q) and continuously calculates Q + 1.
3. **Multiplexer:** Decides whether to feed 0000 (if reset), Q + 1 (if enable), or keep the old Q unchanged.

---

### 2. Dissecting `sim_main.cpp` (The Testbench)

```cpp
#include "Vcounter.h"        // C++ class generated by Verilator
#include "verilated.h"
#include "verilated_vcd_c.h"  // For dumping waveform (.vcd) files

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // 1. Create a virtual instance of our hardware chip
    auto top = std::make_unique<Vcounter>();

    // 2. Enable waveform logging
    Verilated::traceEverOn(true);
    auto tfp = std::make_unique<VerilatedVcdC>();
    top->trace(tfp.get(), 99);
    tfp->open("counter.vcd");

    vluint64_t sim_time = 0; // Simulation clock in nanoseconds

    while (sim_time < 80) {
        // Toggle clock every 5ns (Period = 10ns -> 100MHz clock)
        top->clk = (sim_time % 10 < 5) ? 0 : 1;

        // Hold reset button for the first 15ns, then release it
        top->reset = (sim_time < 15) ? 1 : 0;
        top->enable = 1;

        // Propagate electrical signals across the circuit
        top->eval();

        // Record the voltage of every wire at this nanosecond
        tfp->dump(sim_time);

        sim_time++;
    }

    tfp->close();
    return 0;
}
```

---

## 🛠️ Step 1: Install Verilator & GTKWave

If you haven't installed them yet on Ubuntu / Debian / WSL2:
```bash
sudo apt update
sudo apt install -y verilator gtkwave
```

---

## 🚀 Step 2: Compile & Run the Default Counter

Run the build script from this directory:
```bash
make run
```

### What you will see:
```text
====================================================
       VERILATOR 4-BIT COUNTER SIMULATION           
====================================================
Time(ns)  CLK  RESET  ENABLE  COUNT(Hex)  COUNT(Dec)
----------------------------------------------------
      15    1     0      1        0x0        0
      25    1     0      1        0x1        1
      35    1     0      1        0x2        2
      45    1     0      1        0x3        3
      55    1     0      1        0x4        4
      65    1     0      1        0x5        5
      75    1     0      1        0x6        6
----------------------------------------------------
```

---

## 📊 Step 3: Inspect the Waveform in GTKWave

```bash
make wave
# or: gtkwave counter.vcd
```

### In GTKWave:
1. In the **SST panel** (top left), click `TOP` -> `counter`.
2. Highlight all signals (`clk`, `reset`, `enable`, `count[3:0]`) and click **Append**.
3. Click the **Zoom Fit** icon (magnifying glass with blue square) to view the full signal timeline.

---

## 💡 Step-by-Step Guide to Practice Challenges

Here are 3 guided challenges to help you master Verilog and Verilator. Follow these step-by-step guides!

---

### 🎯 Challenge 1: Build an Up / Down Counter

**Goal:** Add a new input pin `down`. When `down == 1`, the counter counts down (`count - 1`). When `down == 0`, it counts up (`count + 1`).

#### Step 1: Update `counter.v`
1. Add `input wire down` to the module port list:
   ```verilog
   module counter (
       input  wire       clk,
       input  wire       reset,
       input  wire       enable,
       input  wire       down,     // <-- NEW PIN: 1 = count down, 0 = count up
       output reg  [3:0] count
   );
   ```
2. Update the counting logic inside `always @(posedge clk)`:
   ```verilog
   always @(posedge clk) begin
       if (reset) begin
           count <= 4'b0000;
       end else if (enable) begin
           if (down) begin
               count <= count - 4'b0001; // Count DOWN
           end else begin
               count <= count + 4'b0001; // Count UP
           end
       end
   end
   ```

#### Step 2: Update `sim_main.cpp` Testbench
Drive the new `down` pin in your C++ loop so you can test both directions:
```cpp
// Inside the while loop in sim_main.cpp:
// Count UP for the first 50ns, then count DOWN after 50ns:
top->down = (sim_time >= 50) ? 1 : 0;
```

#### Step 3: Recompile and Run
```bash
make run
```
*Observe the counter increment to `0x4` and then start decrementing back down to `0x3`, `0x2`, etc.!*

---

### 🎯 Challenge 2: Expand to an 8-Bit Counter (0 to 255)

**Goal:** Increase the counter resolution from 4 bits (0 to 15) to 8 bits (0 to 255).

#### Step 1: Update `counter.v`
1. Change the output register width from `[3:0]` (4 bits) to `[7:0]` (8 bits):
   ```verilog
   output reg [7:0] count
   ```
2. Update literal values:
   - Reset value: `8'b00000000` (or `8'd0` or `8'h00`).
   - Increment value: `8'b00000001` (or `8'd1`).

#### Step 2: Update `sim_main.cpp`
Change the simulation loop duration from `sim_time < 80` to `sim_time < 300` so you can watch it count higher!

#### Step 3: Recompile and Run
```bash
make run
```

---

### 🎯 Challenge 3: Build a Decade (Mod-10) Counter (0 to 9)

**Goal:** In digital clocks and decimal displays, you need a counter that counts from `0` to `9`, and rolls back to `0` on the 10th tick.

#### Step 1: Update `counter.v`
Modify the `enable` block to check if `count` has reached 9 (`4'd9` or `4'b1001`):
```verilog
always @(posedge clk) begin
    if (reset) begin
        count <= 4'b0000;
    end else if (enable) begin
        if (count == 4'd9) begin
            count <= 4'b0000; // Roll over back to 0!
        end else begin
            count <= count + 4'b0001;
        end
    end
end
```

#### Step 2: Recompile and Run
```bash
make run
```
*Watch the output count: `7 -> 8 -> 9 -> 0 -> 1 -> 2...`*

---

## 🏆 Checklist for Mastery

You have mastered Section 1 Verilator emulation when you can:
- [ ] Read Verilog port lists (`input wire`, `output reg [3:0]`).
- [ ] Understand how `always @(posedge clk)` maps directly to hardware D Flip-Flops.
- [ ] Drive inputs in a C++ testbench (`top->clk`, `top->reset`, `top->eval()`).
- [ ] Inspect `.vcd` waveforms in GTKWave.

---

## 🧭 Navigation
| ⬅️ Previous Guide | 🏠 Overview | 🧪 Alternative Lab |
| :--- | :---: | ---: |
| [⬅️ 04. Anatomy of a Microchip](../04_anatomy_of_a_chip.md) | [Section 1 Hub](../README.md) | [🧪 C++ Toy Simulator ➡️](../conceptual_toy_emulator/README.md) |

