# 03. Hardware Emulation: Simulating Silicon in Software

> *"Verilog is not code that executes; it is a blueprint for physical wires and switches."*

---

## 1. Why Emulate Hardware in Software?

In real hardware (a physical chip or FPGA board):
- If your CPU crashes or locks up, you can't easily "print debug" inside a register without dedicated JTAG probes or logic analyzers.
- Synthesizing a large FPGA design can take **15 to 60 minutes** for a single change!
- Buying hardware costs money and limits who can learn.

**Hardware Emulation with Verilator solves all of this:**
1. **Instant Compilation:** Verilator compiles designs in seconds.
2. **100% Visibility:** Every single wire, register, and internal bus inside your CPU can be recorded and inspected cycle-by-cycle in a waveform viewer.
3. **Automated Testing:** You can write C++ or Python unit tests that assert expected register values at specific clock cycles.

---

## 2. Software Mindset vs Hardware Mindset

Before writing hardware code, you must shift how you think:

```
        SOFTWARE (Sequential)                       HARDWARE (Spatial & Parallel)
 
      Line 1: x = a + b;                     [ Adder 1: x = a + b ]  (Always running!)
              │                                      │
              ▼                                      │ (Simultaneously)
      Line 2: y = c * d;                     [ Multiplier 2: y = c * d ] (Always running!)
              │                                      │
              ▼                                      ▼
      (One instruction at a time)            (All circuits active at all times)
```

In software (C/Python):
- Code runs line by line on a single processor core.
- Variables hold values in memory locations.

In hardware (Verilog/VHDL):
- Every block of code describes physical circuits and wires existing in space simultaneously.
- When input $A$ changes, its effect propagates down the wire instantaneously.

---

## 3. The Two Worlds of Verilog: Combinational vs Sequential

In Verilog, digital logic is split into two categories:

### A. Combinational Logic (Instantaneous)
Outputs change immediately as inputs change (no clock needed).
```verilog
// Continuous assignment (wiring)
assign sum = a ^ b;
assign carry = a & b;

// Or combinational always block
always @(*) begin
    if (sel == 1'b1)
        out = input_b;
    else
        out = input_a;
end
```

### B. Sequential Logic (Clocked / State-Holding)
Registers that update only on a clock transition (rising edge $\uparrow$).
```verilog
// Triggers only when clock goes from 0 to 1
always @(posedge clk or posedge reset) begin
    if (reset) begin
        counter <= 4'd0;         // Reset to 0
    end else if (enable) begin
        counter <= counter + 1;  // Increment state
    end
end
```

> **Crucial Rule:**
> - In combinational blocks, use **blocking assignment (`=`)**.
> - In sequential (clocked) blocks, use **non-blocking assignment (`<=`)** so all registers update in parallel without race conditions.

---

## 4. How Verilator Works Under the Hood

Unlike traditional simulators that interpret Verilog line-by-line during simulation (which is slow), **Verilator is a compiler**:

```
 [ Verilog Code (.v) ]
          │
          ▼  (Verilator)
 [ C++ Class Files (.cpp / .h) ]  <-- Transformed into C++ logic functions!
          │
          ▼  (GCC / Clang)
 [ Fast Native Machine Binary (x86_64 / ARM) ]
```

Verilator takes your Verilog modules and turns them into a high-speed C++ class where:
- Every Verilog wire/register becomes a C++ variable (`vluint32_t`, `uint8_t`, etc.).
- The combinational logic becomes optimized C++ boolean arithmetic.
- The sequential logic updates when you call `top->eval()`.

---

## 5. The Anatomy of a Verilator C++ Testbench

A typical C++ testbench (`sim_main.cpp`) looks like this:

```cpp
#include "Vcounter.h"       // Generated class for module "counter"
#include "verilated.h"
#include "verilated_vcd_c.h" // For dumping waveform files

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    // 1. Instantiate the Verilog hardware module
    Vcounter* top = new Vcounter;

    // 2. Enable waveform tracing (.vcd file)
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("waveform.vcd");

    vluint64_t main_time = 0;

    // 3. The Clock / Simulation Loop
    while (main_time < 200) {
        // Toggle the clock: 0 -> 1 -> 0 -> 1 ...
        top->clk = (main_time % 2 == 1);

        // Drive inputs (e.g. disable reset after time 10)
        top->reset = (main_time < 10) ? 1 : 0;
        top->enable = 1;

        // Evaluate the circuit!
        top->eval();

        // Write the current state to the waveform file
        tfp->dump(main_time);
        main_time++;
    }

    // 4. Cleanup
    tfp->close();
    delete top;
    return 0;
}
```

---

## 6. Visualizing Waves: What is a VCD file?

* A **VCD (Value Change Dump)** file records every transition of every signal at every timestamp.
* When you open `waveform.vcd` in **GTKWave** or **Surfer**, you see a graphical timeline:

```
Time (ns):   0    10   20   30   40   50   60   70
             │    │    │    │    │    │    │    │
clk:         ┌┐   ┌┐   ┌┐   ┌┐   ┌┐   ┌┐   ┌┐   ┌┐
             └┴───┴┴───┴┴───┴┴───┴┴───┴┴───┴┴───┴┴─
reset:       ─────┐
                  └─────────────────────────────────
counter:     0    │ 0  │ 1  │ 2  │ 3  │ 4  │ 5  │ 6
```

---

👉 **Next Step:**
1. Try the **[`conceptual_toy_emulator/`](./conceptual_toy_emulator/)** to see transistors $\to$ gates $\to$ counter running in pure C++ immediately.
2. Run the **[`lab_verilator/`](./lab_verilator/)** to compile real Verilog and generate your first `.vcd` waveform!
