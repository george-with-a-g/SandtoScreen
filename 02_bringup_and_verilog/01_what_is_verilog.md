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
reg q;

always @(posedge clk) begin
    if (reset) begin
        q <= 1'b0;      // Reset Q to 0 on rising clock edge
    end else begin
        q <= d;         // Capture input D into Q on rising clock edge
    end
end
```
* The `@(posedge clk)` tells the compiler: *"Create a physical D Flip-Flop! Only update `q` on the rising edge of the clock wire (0V to 5V)."*

#### B. Combinational Procedural Logic (Creates MUXes, Decoders, ALUs)
```verilog
// 4-to-1 Multiplexer written with a clean case statement
reg [3:0] out;
wire [1:0] sel;
wire [3:0] in0, in1, in2, in3;

always @(*) begin
    case (sel)
        2'b00: out = in0;
        2'b01: out = in1;
        2'b10: out = in2;
        2'b11: out = in3;
        default: out = 4'b0000;
    endcase
end
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

## 🎯 Summary Checklist: Thinking in Silicon

1. **Think in parallel:** Every module and `assign` statement is alive and computing simultaneously.
2. **Every wire has a bit width:** Always know if a wire is 1-bit (`wire clk`) or multi-bit bus (`wire [3:0] count`).
3. **Clocks create memory:** If you want memory that holds a value, use `always @(posedge clk)`.
4. **Gates create instant math:** If you want instant logic, use `assign` or `always @(*)`.

---

## 🧭 Navigation
| ⬅️ Previous Section | 🏠 Section 2 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ Section 1 Hub](../01_intro_and_emulation/README.md) | [Section 2 Hub](./README.md) | [02. Blocking vs Non-Blocking ➡️](./02_blocking_vs_nonblocking.md) |
