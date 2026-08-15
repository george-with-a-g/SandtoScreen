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

## 3. Adding Memory: The Flip-Flop & The Clock Deep Dive

A Look-Up Table (LUT) is brilliant at making instant decisions, but it suffers from **instant amnesia**:
- As soon as you change its inputs, its output changes.
- It has **zero memory** of what happened one microsecond ago.

To build a real computer (like storing a variable, running a line of code, or counting 1, 2, 3...), we need circuits that can **remember** and **keep time**.

---

### A. How Can Electricity "Remember" Anything? (The Feedback Loop)

Imagine a standard buzzer or light. When you take your finger off the button, the light turns off. 

How can we make a circuit stay ON even after we release the button?
**By looping the output wire back into the input!**

```
              ┌─────────┐
    Input ───►│  OR     ├──────┬──────► Output (Light stays ON!)
              │  Gate   │      │
          ┌──►│         │      │
          │   └─────────┘      │
          │                    │
          └────────────────────┘ (Feedback Loop: Output feeds itself!)
```

1. You press the button once (`Input = 1`).
2. The OR gate outputs `1`.
3. That `1` travels through the feedback wire right back into the OR gate.
4. Now, even if you let go of the button (`Input = 0`), the OR gate is feeding itself `1` forever!

This is the birth of digital memory (called a **Latch**).

---

### B. The "Runaway Loop" Problem (Why We Need a Clock)

Now imagine you want to build a simple **Counter** that adds 1:
```text
New Value = Old Value + 1
```

If you connect the output of an adder directly back to its input without a barrier, something disastrous happens:

```
    ┌────────────────────────────────────────────────────────┐
    ▼                                                        │
 [ Adder (+1) ] ──► (0 becomes 1) ──► (1 becomes 2) ──► (2 becomes 3) ...
```

Because electricity travels at nearly the speed of light, the numbers spin uncontrollably:
```text
0 -> 1 -> 2 -> 3 -> 999999 (in a few nanoseconds!)
```

You cannot control it. This is called a **Race Condition**. 

To fix this, we need a **Turnstile / Air Lock** that only lets ONE value through at a time on a precise beat.

---

### C. The Clock: The Metronome of the Chip

A **Clock** is a physical wire connected to a quartz crystal that rhythmically toggles between `0` and `1` millions (or billions) of times every second:

```
Voltage
  1 (HIGH) ──┐         ┌─────────┐         ┌─────────┐
             │         │         │         │         │
  0 (LOW)  ──┴─────────┘         └─────────┘         └─────────
             ▲                   ▲                   ▲
        Rising Edge         Rising Edge         Rising Edge
       (TICK #1)           (TICK #2)           (TICK #3)
```

The most important moment is the **Rising Edge** (the exact instant the signal jumps from `0` to `1`). That is the universal "GO!" signal for the entire chip.

---

### D. The D Flip-Flop: The 1-Bit Snapshot Camera

A **D Flip-Flop** is the turnstile that solves the runaway problem.

```
                  ┌────────────┐
   Input (D) ────►│ D        Q ├────► Output (Q) [Holds stored value]
                  │            │
   Clock (CLK) ──►│ >          │
                  └────────────┘
```

- **Input D (Data):** The new value waiting outside the door.
- **Clock CLK:** The door control.
- **Output Q:** The saved value currently inside.

#### How It Works:
1. **Between Clock Ticks:** The door is **locked tight**. Whatever is happening at input `D` is completely ignored. Output `Q` remains rock-solid and stable.
2. **On the Rising Edge (Tick 0 -> 1):** The door unlocks for a split picosecond. It takes a **snapshot** of input `D`, updates output `Q`, and immediately locks the door again!

---

### E. Putting It Together: A Working 1-Step Counter

Now look at how clean and controlled counting becomes when we pair an **Adder** with a **Flip-Flop**:

```
                  ┌────────────┐
      ┌──────────►│ D        Q ├────┬───► Current Count Output
      │           │ Flip-Flop  │    │
      │   Clock ─►│ >          │    │
      │           └────────────┘    │
      │                             │
      └─────────[ Adder (+1) ]◄─────┘
```

Let's trace it step-by-step:

1. **Start:** The Flip-Flop is holding `0` (Output Q = 0).
2. **Thinking Time:** The Adder sees `0` on output Q, calculates 0 + 1 = 1, and places `1` at input wire D.
   - *The Flip-Flop door is still locked, so output Q stays `0`.*
3. **CLOCK TICK #1:**
   - The Flip-Flop captures the `1` from wire D.
   - Output Q becomes **`1`**.
   - Door locks shut.
4. **Thinking Time:** The Adder now sees Q = 1, calculates 1 + 1 = 2, and places `2` at wire D.
   - *The `2` waits patiently outside the locked door.*
5. **CLOCK TICK #2:**
   - The Flip-Flop captures the `2`.
   - Output Q becomes **`2`**.
   - Door locks shut.

Now the computer counts cleanly: **0, 1, 2, 3... exactly one step per clock tick!**

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
