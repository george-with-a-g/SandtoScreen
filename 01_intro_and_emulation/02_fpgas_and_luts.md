# 02. FPGAs and Look-Up Tables: Programmable Silicon Made Simple

> **Goal:** Understand how a single physical chip can magically turn into an ARM processor, an arcade machine, or an AI accelerator without touching a soldering iron.

---

## 📖 Jargon Buster (Read this first!)

| Jargon Term | What it actually means in plain English |
| :--- | :--- |
| **FPGA** | *(Field-Programmable Gate Array)* A blank chip containing millions of tiny configurable cheat sheets and digital switches that you can reprogram anytime. |
| **ASIC** | *(Application-Specific Integrated Circuit)* A permanent, custom-made chip (like an iPhone's A17 chip). Cannot be changed after factory manufacturing. |
| **LUT (Look-Up Table)** | A tiny **cheat sheet memory**. Instead of calculating math with physical gates, it simply looks up the pre-calculated answer in a table. |
| **Multiplexer (MUX)** | A **digital train track switch**. It has multiple input tracks, but only one output track. A control dial picks which input goes through. |
| **Flip-Flop** | A **1-bit memory cell**. Think of it as a snapshot camera that captures a `0` or `1` on every tick of the clock and remembers it. |
| **Clock** | A steady **metronome / heartbeat** (ticking millions of times per second) that synchronizes all actions across the entire computer. |
| **Bitstream** | The configuration binary file (a stream of 1s and 0s) that is loaded into the FPGA to program all the cheat sheets and wire connections. |

---

## 1. The Big Problem: Silicon Chips Are Hard to Change

When a factory manufactures an Intel or Apple CPU:
- Microscopic wires and transistors are permanently etched onto a silicon wafer.
- If an engineer finds a single hardware bug, the company has to spend millions of dollars and wait 6 months to make a new chip.

**What if we could have a chip that can change its internal circuits in seconds?**

That chip is an **FPGA**.

---

## 2. The Big Secret: The Look-Up Table (LUT)

An FPGA does **not** physically melt or rewire its silicon. 

Instead, it replaces hardwired logic gates with a clever trick: **The Look-Up Table (LUT)**.

### The Restaurant Menu Analogy
Imagine a restaurant cashier who has to calculate bills.
- **Approach A (Hardwired Gate):** The cashier performs manual multiplication and addition on paper every single time.
- **Approach B (Look-Up Table):** The cashier keeps a **cheat sheet card** on the counter:
  - If customer picks Combo 1 -> Charge 5 dollars
  - If customer picks Combo 2 -> Charge 8 dollars
  - If customer picks Combo 3 -> Charge 12 dollars

The cashier doesn't do any math! They just **look up** the pre-written answer.

---

### How a 2-Input LUT (LUT2) Works

Suppose we want to build an **AND gate** (A AND B):

```
 Truth Table:
 A | B | Result
---|---|-------
 0 | 0 |   0    <-- Stored in memory Slot 0
 0 | 1 |   0    <-- Stored in memory Slot 1
 1 | 0 |   0    <-- Stored in memory Slot 2
 1 | 1 |   1    <-- Stored in memory Slot 3
```

Inside the FPGA, a **2-Input LUT** consists of:
1. **4 tiny storage bits (SRAM)** holding the values `0, 0, 0, 1`.
2. A **Multiplexer (Selector switch)** controlled by Input A and Input B:

```
  Stored Bits in Memory
     [ 0 ] ── Slot 0 ──┐
     [ 0 ] ── Slot 1 ──┤
     [ 0 ] ── Slot 2 ──┼──[ Multiplexer ]─── Output
     [ 1 ] ── Slot 3 ──┘         ▲
                                 │
                          Inputs A and B
                     (Select which slot to read)
```

- If you set A=1 and B=1, the selector points to **Slot 3** and outputs **`1`**.
- If you set A=0 and B=1, the selector points to **Slot 1** and outputs **`0`**.

### What if you want an OR gate instead?
You don't change the hardware! You simply overwrite the 4 memory slots with `0, 1, 1, 1`.
Now the exact same silicon is an **OR gate**!

> **Key Takeaway:** An FPGA implements any logic function simply by loading its truth table into memory slots.

---

## 3. Adding Memory: The Flip-Flop & The Clock

A LUT can make decisions, but it cannot remember the past. As soon as its inputs change, its output changes.

To build a real computer (like a counter or a CPU), we need **Memory** and **Time**.

### The Clock: The Conductor of the Orchestra
A **Clock** is a wire that continuously alternates between `0` and `1` like a metronome:
```
  1 ──┐   ┌──┐   ┌──┐   ┌──┐
      │   │  │   │  │   │  │
  0 ──┴───┘  └───┘  └───┘  └───
       Tick   Tick   Tick   Tick
```
Every time the clock ticks (rises from `0` to `1`), the computer takes one coordinated step forward.

---

### The D Flip-Flop: The 1-Bit Camera

A **D Flip-Flop** is a 1-bit memory box:
- It has a **Data Input (D)**, a **Clock Input (CLK)**, and an **Output (Q)**.
- **Rule:** On the exact instant the clock ticks (rising edge 0 -> 1), the Flip-Flop takes a "snapshot" of input `D` and holds that value at output `Q` until the next clock tick.

```
                  ┌────────────┐
   Input (D) ────►│ D        Q ├────► Output (Q) [Remembers state]
                  │            │
   Clock (CLK) ──►│ >          │
                  └────────────┘
```

By connecting the output of a Flip-Flop back into a LUT (with an adder rule), we get a **Counter**:
- Tick 1: Output is `0` -> Next state calculated is `1`.
- Tick 2: Flip-flop captures `1` -> Next state calculated is `2`.
- Tick 3: Flip-flop captures `2` -> Next state calculated is `3`.

---

## 4. Inside a Logic Cell: The Basic Lego Brick

An FPGA contains thousands of identical building blocks called **Logic Cells** (or Configurable Logic Blocks - CLBs).

Each Logic Cell has:
1. A **LUT** (to make logic decisions / truth tables).
2. A **D Flip-Flop** (to remember values on clock ticks).
3. A **Switch** (to choose whether to use the instant LUT result or the saved Flip-Flop result).

```
   Inputs ──► [ LUT ] ──┬───────────────[ Switch ]──► Output (Instant)
                        │                     ▲
                        └──► [ Flip-Flop ] ───┘
                                   ▲
       Clock ──────────────────────┘
```

---

## 5. Summary: How it all fits together

| Layer | What it does | Real-world equivalent |
| :--- | :--- | :--- |
| **LUT (Look-Up Table)** | Computes logic rules | A restaurant order cheat sheet |
| **D Flip-Flop** | Remembers values across clock ticks | A snapshot camera |
| **Logic Cell** | Combines 1 LUT + 1 Flip-Flop | A single Lego brick |
| **FPGA** | Millions of Logic Cells connected by programmable wires | A giant box of Lego bricks you can build anything from |

---

👉 Next Step: Read **[`03_hardware_emulation.md`](./03_hardware_emulation.md)** to see why we simulate this hardware in software (Verilator) on your laptop!
