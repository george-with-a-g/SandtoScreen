# 02. Blocking vs. Non-Blocking: The #1 Rule of Verilog

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](./README.md) • [01. Thinking in Verilog](./01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](./02_blocking_vs_nonblocking.md) • [03. FSM Design](./03_fsm_design.md) • [💡 Lab 1: Blinky](./lab_blinky/README.md) • [📡 Lab 2: UART](./lab_uart/README.md)
---

The single most common mistake software engineers make in Verilog is confusing **Blocking Assignments (`=`)** with **Non-Blocking Assignments (`<=`)**.

Getting this wrong causes **race conditions, simulation mismatches, and broken hardware synthesis**. 

Fortunately, there are **two simple rules** that guarantee you will never write broken Verilog.

---

## 🏎️ The Two Types of Assignments

```
                ┌─────────────────────────────────────────────────┐
                │          VERILATOR / SILICON ASSIGNMENTS        │
                ├────────────────────────┬────────────────────────┤
                │   BLOCKING ('=')       │  NON-BLOCKING ('<=')   │
                ├────────────────────────┼────────────────────────┤
                │ • Evaluates instantly  │ • Evaluates all RHS    │
                │   line-by-line         │   simultaneously       │
                │ • Blocks next line     │ • Updates all LHS      │
                │ • Used for GATES &     │   together on clock    │
                │   combinational logic  │ • Used for FLIP-FLOPS  │
                └────────────────────────┴────────────────────────┘
```

---

## 🔍 The Classic Shift Register Experiment

Imagine you want to build a **2-stage Shift Register** (a pipeline where data moves from Flip-Flop 1 to Flip-Flop 2 on each clock tick):

```
       Input A ──► [ Flip-Flop 1 (reg B) ] ──► [ Flip-Flop 2 (reg C) ] ──► Output C
```
* **Tick 1:** Data enters `B`. `C` still holds the old value.
* **Tick 2:** Data moves from `B` to `C`.

Let's see what happens if you write this in Verilog using `=` vs `<=`:

---

### ❌ The WRONG Way: Using Blocking (`=`) in Clocked Logic

```verilog
always @(posedge clk) begin
    b = a;      // Line 1: b immediately gets the new value of a
    c = b;      // Line 2: c immediately gets the NEW value of b!
end
```

#### What happens physically in simulation:
1. When the clock rises, Line 1 executes: `b` immediately becomes `a`.
2. Line 2 executes next: `c` immediately reads `b` (which was JUST updated to `a`)!
3. **The Disaster:** In **one single clock tick**, `c` received `a` directly! 
4. The synthesizer realizes `b` is redundant and synthesizes **ONLY ONE Flip-Flop instead of two**! Your 2-stage pipeline is destroyed.

---

### ✅ The RIGHT Way: Using Non-Blocking (`<=`) in Clocked Logic

```verilog
always @(posedge clk) begin
    b <= a;     // Line 1: scheduled to update at end of tick
    c <= b;     // Line 2: reads the OLD value of b!
end
```

#### What happens physically in silicon:
1. When the clock rises, the hardware takes a **snapshot** of all inputs:
   - It reads current `a`.
   - It reads current `b` (the OLD value stored in Flip-Flop 1).
2. At the end of the clock tick, it commits all updates simultaneously:
   - `b` receives the snapshot of `a`.
   - `c` receives the snapshot of the OLD `b`.
3. **The Success:** Data shifts from `a` to `b`, and from `b` to `c` in two true clock cycles. Two real physical Flip-Flops are synthesized!

---

## 📜 The Golden Rules of Verilog (Memorize These!)

| What You Are Building | Verilog Block to Use | Assignment Operator to Use |
| :--- | :--- | :---: |
| **Clocked Sequential Logic** (Flip-Flops, Registers, State Machines) | `always @(posedge clk)` | **`<=` (Non-Blocking)** |
| **Combinational Logic** (ALUs, Multiplexers, Decoders, Math) | `always @(*)` | **`=` (Blocking)** |
| **Continuous Wiring** | `assign ...` | **`=` (Continuous)** |

---

### 🚨 Never Mix Both in the Same `always` Block!
```verilog
// ❌ ILLEGAL / DANGEROUS: Mixing = and <=
always @(posedge clk) begin
    temp = a + b;   // DANGEROUS
    out <= temp;
end

// ✅ CLEAN & CORRECT: Split into Combinational + Sequential
wire [3:0] temp;
assign temp = a + b;            // Combinational adder

always @(posedge clk) begin
    out <= temp;                // Clocked Flip-Flop capturing the sum
end
```

---

## 🎯 Summary Checklist:
1. **Clocked with `posedge clk`?** Always use **`<=`**.
2. **Combinational with `always @(*)` or `assign`?** Always use **`=`**.
3. Following these two rules guarantees zero simulation-synthesis mismatches.

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 2 Hub | ➡️ Next Chapter |
| :--- | :---: | ---: |
| [⬅️ 01. Thinking in Verilog](./01_what_is_verilog.md) | [Section 2 Hub](./README.md) | [03. FSM Design ➡️](./03_fsm_design.md) |
