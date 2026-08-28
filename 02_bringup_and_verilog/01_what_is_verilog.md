# 01. Thinking in Verilog: The Fundamental Mental Shift

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](./README.md) • [01. Thinking in Verilog](./01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](./02_blocking_vs_nonblocking.md) • [03. FSM Design](./03_fsm_design.md) • [💡 Lab 1: Blinky](./lab_blinky/README.md) • [📡 Lab 2: UART](./lab_uart/README.md)
---

When programmers transition from software (Python, C, JavaScript) to hardware, they often struggle because they try to read Verilog like code that runs line-by-line.

**Verilog is NOT software.** Verilog is a **Hardware Description Language (HDL)**. You are not writing a list of instructions for a CPU to execute — **you are drawing a circuit diagram with text!**

---

## 🍳 The Chef vs. The Factory (Software vs. Hardware)

```
SOFTWARE (Python / C):
  ┌─────────────────────────────────────────────────────────────┐
  │ A Single Chef in a Kitchen (The CPU)                        │
  │ • Line 1: Chop carrots                                      │
  │ • Line 2: Boil water                                        │
  │ • Line 3: Add salt                                          │
  │ Executed strictly ONE instruction after another in order!   │
  └─────────────────────────────────────────────────────────────┘

HARDWARE (Verilog HDL):
  ┌─────────────────────────────────────────────────────────────┐
  │ A Massive Factory of 10,000 Workers (The Silicon Die)       │
  │ • Worker 1 is ALWAYS chopping carrots                       │
  │ • Worker 2 is ALWAYS boiling water                          │
  │ • Worker 3 is ALWAYS adding salt                            │
  │ Everything happens SIMULTANEOUSLY in parallel, all at once! │
  └─────────────────────────────────────────────────────────────┘
```

In a real microchip, electricity flows through every copper trace and transistor at the same time. If you write 50 equations in Verilog, **all 50 equations are computing at the exact same nanosecond**!

---

## 🧱 The Anatomy of a Verilog Module

In Verilog, every circuit is packaged inside a **`module`** (just like an integrated circuit chip with physical metal pins sticking out of it):

```
                       ┌──────────────────────┐
       Input Pin A ───►│                      │
                       │   MY_CUSTOM_CHIP     ├───► Output Pin Y
       Input Pin B ───►│                      │
                       └──────────────────────┘
```

Here is how that chip looks in Verilog code:

```verilog
module my_custom_chip (
    input  wire a,      // Input pin A (1-bit electrical wire)
    input  wire b,      // Input pin B (1-bit electrical wire)
    output wire y       // Output pin Y (1-bit electrical wire)
);

    // Internal circuit: An AND gate connecting A and B to Y
    assign y = a & b;

endmodule
```

### Key Rules of Module Structure:
1. **`module <name> (...)`**: Starts the definition and lists all input/output pins.
2. **`input` / `output`**: Specifies whether electricity flows INTO or OUT OF the chip pin.
3. **`endmodule`**: Closes the chip definition.

---

## ⚡ The Two Data Types: `wire` vs `reg`

Understanding the difference between `wire` and `reg` is the first milestone in mastering Verilog:

| Feature | `wire` (Physical Copper Trace) | `reg` (Procedural Driver / Storage) |
| :--- | :--- | :--- |
| **What it represents** | A physical conductive wire connecting two points. | A variable driven inside an `always` procedural block. |
| **Has memory?** | ❌ **No**. Holds no charge; if you cut input, voltage vanishes. | ⚠️ **Can be memory** (Flip-Flop) if clocked, or pure logic if combinational. |
| **How you assign it** | With continuous `assign` statements. | Inside `always @(...)` blocks. |
| **Example** | `assign sum = a ^ b;` | `always @(posedge clk) q <= d;` |

> **Crucial Insight:** In modern Verilog (SystemVerilog), people often use `logic` to replace both, but understanding `wire` (direct physical connection) vs `reg` (procedural assignment) will make you understand the underlying silicon synthesis.

---

## 🔄 Continuous Assignment vs. Procedural Blocks

There are two primary ways to describe logic in Verilog:

### 1. Continuous Assignment (`assign`) — Pure Combinational Flow
Used for logic gates and simple math where output changes **immediately** when any input changes:

```verilog
// 1-Bit Multiplexer built with boolean logic
wire a, b, sel;
wire out;

assign out = (a & ~sel) | (b & sel);
```
* As soon as voltage on `sel`, `a`, or `b` changes, voltage on `out` updates **instantly** (within transistor propagation delay).

---

### 2. Procedural Block (`always`) — Clocked and Complex Logic
An `always` block runs whenever the signals in its **sensitivity list** change.

#### A. Clocked Sequential Logic (Creates D Flip-Flops!)
```verilog
// 1-Bit D Flip-Flop with Active-High Synchronous Reset
module d_flip_flop (
    input  wire clk,    // Clock signal
    input  wire reset,  // Active-high reset
    input  wire d,      // Data input
    output reg  q       // Registered output (Q)
);

    always @(posedge clk) begin
        if (reset) begin
            q <= 1'b0;  // Reset Q to 0 on rising clock edge
        end else begin
            q <= d;     // Capture input D into Q on rising clock edge
        end
    end

endmodule
```
* The `@(posedge clk)` tells the compiler: *"Create a physical D Flip-Flop! Only update `q` on the rising edge of the clock wire (0V to 5V)."*

#### B. Combinational Procedural Logic (Creates MUXes, Decoders, ALUs)
```verilog
// 4-to-1 Multiplexer written with a clean case statement
module mux4 (
    input  wire [1:0] sel,             // 2-bit selection wire
    input  wire [3:0] in0, in1, in2, in3, // Four 4-bit data candidates
    output reg  [3:0] out              // 4-bit selected output
);

    always @(*) begin
        case (sel)
            2'b00: out = in0;
            2'b01: out = in1;
            2'b10: out = in2;
            2'b11: out = in3;
            default: out = 4'b0000;
        endcase
    end

endmodule
```
* The `@(*)` means *"wake up whenever ANY input changes"*. Because there is no clock, this synthesizes to pure logic gates and multiplexers (no Flip-Flops).

---

## 🔢 Understanding Verilog Number Literals

In Verilog, numbers are written with a specific syntax to declare their **exact bit width** and **number base**:

```
        4 'b 1010
        │  │  └── The actual digits (1010)
        │  └───── The base: 'b (binary), 'd (decimal), 'h (hexadecimal)
        └──────── The width in bits (4 bits wide)
```

| Literal | Bit Width | Base | Decimal Value | Binary Representation |
| :--- | :---: | :---: | :---: | :---: |
| `1'b1` | 1 | Binary | 1 | `1` |
| `4'b1010` | 4 | Binary | 10 | `1010` |
| `8'd255` | 8 | Decimal | 255 | `11111111` |
| `8'hFF` | 8 | Hexadecimal | 255 | `11111111` |
| `32'hDEAD_BEEF` | 32 | Hexadecimal | 3,735,928,559 | `11011110101011011011111011101111` |

*(Underscores `_` are ignored by the compiler and are purely for human readability!)*

---

## 🔬 How to "Run" and Simulate Any Verilog Module

A common question from software engineers is: *"How do I run a Verilog file like `inverter.v` in my terminal?"*

In software (C++, Python), programs have a `main()` entry point that starts executing line-by-line.  
In hardware, **Verilog modules do NOT have a `main()`**. A Verilog module is an isolated silicon chip with metal pins. Sitting on a table by itself, it has no power supply, no input signals, and no screen to print text.

To **"run"** a Verilog module, you attach it to a **Testbench** (a virtual lab bench with signal generators and an oscilloscope).

---

### 🌟 Method 1: The Verilator C++ Workflow (Used in this Course)

Verilator translates your Verilog module into a **C++ class** (`Vinverter.h`). You write a small C++ testbench to wiggle the input pins and measure the output pins.

#### 1. The Circuit (`inverter.v`):
```verilog
module inverter (
    input  wire in,
    output wire out
);
    assign out = ~in;
endmodule
```

#### 2. The Virtual C++ Lab Bench (`sim_inverter.cpp`):
```cpp
#include <iostream>
#include "Vinverter.h" // Generated by Verilator

int main(int argc, char** argv) {
    Vinverter top;

    // Test 1: Put 0V on the 'in' pin
    top.in = 0;
    top.eval(); // Calculate circuit voltages
    std::cout << "Input: 0 --> Output: " << (int)top.out << " (Expected: 1)\n";

    // Test 2: Put 5V on the 'in' pin
    top.in = 1;
    top.eval(); // Calculate circuit voltages
    std::cout << "Input: 1 --> Output: " << (int)top.out << " (Expected: 0)\n";

    return 0;
}
```

#### 3. Terminal Commands to Compile and Run:
```bash
# Step A: Compile Verilog into C++ model and link with your testbench
verilator -Wall -cc inverter.v --exe sim_inverter.cpp

# Step B: Build the executable binary
make -C obj_dir -f Vinverter.mk Vinverter

# Step C: Run the simulation!
./obj_dir/Vinverter
```

---

### 🌟 Method 2: The Pure Verilog Testbench Workflow (`iverilog` / `vvp`)

You can also write your testbench in pure Verilog using special simulation-only keywords like `$display`, `$monitor`, and time delays (`#10`):

#### 1. The Pure Verilog Testbench (`inverter_tb.v`):
```verilog
module inverter_tb;
    reg  tb_in;   // Testbench drives inputs with 'reg'
    wire tb_out;  // Testbench observes outputs with 'wire'

    // Instantiate the chip under test (UUT)
    inverter uut (
        .in(tb_in),
        .out(tb_out)
    );

    initial begin
        $monitor("Time=%0t | in=%b --> out=%b", $time, tb_in, tb_out);

        tb_in = 0;
        #10; // Wait 10 simulation time units

        tb_in = 1;
        #10;

        $finish; // End simulation
    end
endmodule
```

#### 2. Terminal Commands to Run with Icarus Verilog:
```bash
# Compile Verilog module + testbench
iverilog -o sim_inverter inverter.v inverter_tb.v

# Run the simulation engine
vvp sim_inverter
```

---

## 🛠️ Step-by-Step Hands-On Tasks: "Hello World" in Verilog

Now that you know how hardware simulation works, work through these 5 progressive mini-tasks located in [`practice_circuits/`](./practice_circuits/):

---

### 🟢 Task 1: The "Hello World" of Hardware — 1-Bit Inverter (`inverter.v`)

The simplest circuit you can build is a NOT gate:

```verilog
// practice_circuits/inverter.v
module inverter (
    input  wire in,     // 1-bit input wire
    output wire out     // 1-bit output wire
);

    // Bitwise NOT operator in Verilog is '~'
    assign out = ~in;

endmodule
```

#### ❓ Task 1 Challenge:
> **Question:** If `in = 1'b0`, what is the voltage on `out`?  
> *(Test your prediction, then check Answer 1 in the Answer Key at the bottom!)*

---

### 🟢 Task 2: Combinational Logic — 2-to-1 Multiplexer (`mux2.v`)

Selects between input `a` and input `b` based on a select wire `sel`:

```verilog
// practice_circuits/mux2.v
module mux2 (
    input  wire a,      // Selected when sel == 0
    input  wire b,      // Selected when sel == 1
    input  wire sel,    // 1-bit control wire
    output wire y       // Output
);

    // Using the ternary operator: (condition ? if_true : if_false)
    assign y = sel ? b : a;

endmodule
```

#### ❓ Task 2 Challenge:
> **Question:** If `sel = 1'b1`, `a = 1'b0`, and `b = 1'b1`, what does `y` equal?  
> *(Test your prediction, then check Answer 2 in the Answer Key at the bottom!)*

---

### 🟢 Task 3: Memory & Clocking — 1-Bit D Flip-Flop (`dff.v`)

Stores 1 bit of data on the rising clock edge:

```verilog
// practice_circuits/dff.v
module dff (
    input  wire clk,    // Heartbeat clock wire
    input  wire reset,  // Active-high synchronous reset
    input  wire d,      // Data input waiting at the door
    output reg  q       // Registered output (Q)
);

    always @(posedge clk) begin
        if (reset) begin
            q <= 1'b0;  // Reset to 0 on clock tick
        end else begin
            q <= d;     // Capture D into Q on clock tick
        end
    end

endmodule
```

#### ❓ Task 3 Challenge:
> **Question:** If `d = 1'b1`, but the clock is currently sitting at `0V` (LOW), does `q` change?  
> *(Test your prediction, then check Answer 3 in the Answer Key at the bottom!)*

---

### 🟢 Task 4: Arithmetic — 1-Bit Half Adder (`half_adder.v`)

Computes binary addition (A + B = Sum + Carry):

```verilog
// practice_circuits/half_adder.v
module half_adder (
    input  wire a,
    input  wire b,
    output wire sum,    // Single-bit sum (One's place)
    output wire carry   // Carry out (Two's place)
);

    assign sum   = a ^ b; // XOR gate (1 if inputs are different)
    assign carry = a & b; // AND gate (1 only if both inputs are 1)

endmodule
```

#### ❓ Task 4 Challenge:
> **Question:** If `a = 1'b1` and `b = 1'b1`, what are `sum` and `carry`?  
> *(Test your prediction, then check Answer 4 in the Answer Key at the bottom!)*

---

### 🟢 Task 5: Submodule Instantiation (Wiring Chips Together)

How do you connect chips together in Verilog? You **instantiate** submodules like LEGO bricks, connecting their ports with internal `wire`s!

Let's connect **two Inverters in series** to make a non-inverting Buffer (`in -> NOT -> NOT -> out`):

```verilog
// practice_circuits/buffer_series.v
module buffer_series (
    input  wire in,
    output wire out
);

    // Internal wire connecting the output of Inverter 1 to input of Inverter 2
    wire mid_wire;

    // Submodule 1: Inverts 'in' and drives 'mid_wire'
    inverter inv1 (
        .in(in),            // Connect chip pin .in to wire (in)
        .out(mid_wire)      // Connect chip pin .out to wire (mid_wire)
    );

    // Submodule 2: Inverts 'mid_wire' and drives final 'out'
    inverter inv2 (
        .in(mid_wire),      // Connect chip pin .in to wire (mid_wire)
        .out(out)           // Connect chip pin .out to wire (out)
    );

endmodule
```

> **Syntax Tip:** The `.pin_name(connected_wire)` syntax is called **Named Port Mapping**. It guarantees that wires connect to the correct physical pins regardless of the order you write them!

---

## 🧠 Self-Check Quizzes

### ❓ Quiz 1:
> Why can you NOT assign to a `wire` inside an `always @(posedge clk)` block?

### ❓ Quiz 2:
> In Verilog, what is the difference in hardware between `4'd5` and `4'b0101`?

### ❓ Quiz 3:
> If you write 3 separate `assign` statements inside a module, in what order do they execute?

---

## 🎯 Summary Checklist: Thinking in Silicon

1. **Think in parallel:** Every module and `assign` statement is alive and computing simultaneously.
2. **Every wire has a bit width:** Always know if a wire is 1-bit (`wire clk`) or multi-bit bus (`wire [3:0] count`).
3. **Clocks create memory:** If you want memory that holds a value, use `always @(posedge clk)`.
4. **Gates create instant math:** If you want instant logic, use `assign` or `always @(*)`.
5. **Connect chips with named ports:** Always use `.pin(wire)` when instantiating submodules.

---

## 🔑 Answer Key & Deep Explanations

### Task Challenges:
* **Answer 1 (Inverter):** `out = 1'b1` (5V / HIGH). As soon as voltage on `in` drops to 0, `out` inverts to 1 immediately.
* **Answer 2 (MUX2):** `y = 1'b1`. Because `sel == 1`, input `b` is passed to the output.
* **Answer 3 (D Flip-Flop):** **No!** A D Flip-Flop only captures data on the **rising edge** (`posedge clk`: 0V to 5V transition). Between clock edges, `q` remains frozen.
* **Answer 4 (Half Adder):** `sum = 1'b0`, `carry = 1'b1`. Binary `"10"` = Decimal 2 (1 + 1 = 2).

### Quizzes:
* **Answer Quiz 1:** A `wire` represents a pure physical electrical trace with no memory. An `always @(posedge clk)` creates a storage Flip-Flop, which requires a `reg` to hold procedural state between clock edges.
* **Answer Quiz 2:** They are **identical in hardware**! Both create a 4-bit bus carrying the binary value `0101`. The only difference is human notation (`'d` decimal vs `'b` binary).
* **Answer Quiz 3:** **They execute all at the same time (simultaneously)!** In silicon hardware, all 3 equations are permanently wired and active in parallel.

---

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Section 2 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ Section 1 Hub](../01_intro_and_emulation/README.md) | [Section 2 Hub](./README.md) | [02. Blocking vs Non-Blocking ➡️](./02_blocking_vs_nonblocking.md) |
