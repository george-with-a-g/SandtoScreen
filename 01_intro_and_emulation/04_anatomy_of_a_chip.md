# 04. Anatomy of a Microchip: From Transistors to Adders, Multiplexers, and Registers

---
### 🧭 Chapter 1 Quick Links
[🏠 Section 1 Overview](./README.md) • [01. Transistors to Gates](./01_transistors_to_gates.md) • [02. FPGAs & LUTs](./02_fpgas_and_luts.md) • [03. Hardware Emulation](./03_hardware_emulation.md) • [04. Anatomy of a Chip](./04_anatomy_of_a_chip.md) • [🧪 C++ Toy Sim](./conceptual_toy_emulator/README.md) • [🔬 Verilator Lab](./lab_verilator/README.md)
---

> **Goal:** A complete, bottom-up master reference showing how physical transistors form logic gates, how gates form Adders and Multiplexers, how Flip-Flops form Registers, and how they all unite to create a working microchip.

---

## 🏛️ 1. The Grand Silicon Pyramid

Every digital microchip in the world (from a simple 4-bit counter up to an Apple M3 or Intel CPU) is structured in this exact hierarchy:

```
┌────────────────────────────────────────────────────────────────────────┐
│ LEVEL 5: THE COMPLETE CHIP (e.g. counter.v or a CPU)                   │
├────────────────────────────────────────────────────────────────────────┤
│ LEVEL 4: THE 3 CORE HARDWARE BLOCKS:                                   │
│   1. REGISTERS (Memory State)                                          │
│   2. BINARY ADDER (Computation)                                        │
│   3. MULTIPLEXER (Decision Making & Routing)                           │
├────────────────────────────────────────────────────────────────────────┤
│ LEVEL 3: 1-BIT SUB-CIRCUITS:                                           │
│   • Full Adder (Half Adders + OR)                                      │
│   • 1-Bit MUX Slice (AND + OR + NOT)                                   │
│   • D Flip-Flop (Master-Slave Latches)                                 │
├────────────────────────────────────────────────────────────────────────┤
│ LEVEL 2: BASIC LOGIC GATES (NOT, NAND, NOR, AND, OR, XOR)              │
├────────────────────────────────────────────────────────────────────────┤
│ LEVEL 1: PHYSICAL TRANSISTORS (PMOS Pull-Up & NMOS Pull-Down)          │
├────────────────────────────────────────────────────────────────────────┤
│ LEVEL 0: RAW SILICON & VOLTAGE (5V / 1.2V = 1, 0V = 0)                 │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 🔢 2. The 4-Bit Binary Adder (Deep Breakdown)

The **Adder** is the math engine of the chip. In [`counter.v`](./lab_verilator/counter.v), it computes `count + 1`.

---

### Step 2.1: Level 1 — The Half Adder (Adds 2 bits: A + B)
When adding 2 single bits, the math rules are:
* 0 + 0 = 0 (Sum=0, Carry=0)
* 0 + 1 = 1 (Sum=1, Carry=0)
* 1 + 0 = 1 (Sum=1, Carry=0)
* 1 + 1 = 2 (in binary: `10` -> Sum=0, Carry=1)

**The Circuit:**
* **Sum** is produced by **1 XOR Gate** (different inputs = 1).
* **Carry Out** is produced by **1 AND Gate** (both inputs 1 = 1).

```
   Input A ──┬───────────────┐
             │               ├──[ XOR Gate ]───► Sum Output (1 bit)
   Input B ──┼───────────────┤
             │               └──[ AND Gate ]───► Carry Out (to next column)
             └───────────────┘
```
* **Total Components in Half Adder:** 2 Gates = **14 Transistors**.

---

### Step 2.2: Level 2 — The Full Adder: Explained With Real Values

A **Half Adder** is great, but it is "half" because it can only add **two** numbers (A + B).

When you add big numbers on paper, you often have **THREE** numbers to add in a column:
1. Number A (coming from the left register wire)
2. Number B (coming from the right register wire)
3. The **Carry-In** rippling in from the neighbor column on the right!

A **Full Adder** is the machine that adds **all three numbers at once**.

---

#### 🍎 The 3-Apples Story (The Only 4 Possibilities)

Imagine 3 friends: **Friend A**, **Friend B**, and **Friend Carry-In**.
Each friend is either holding **0 apples** or **1 apple**.

How many total apples can there be? There are only **4 possible totals in the universe**:

| Scenario | Total Apples | Carry Out (Two's Box) | Sum (One's Box) | Binary Output (`Carry Sum`) |
| :--- | :---: | :---: | :---: | :---: |
| **No one has an apple** (0 + 0 + 0) | **0** | `0` | `0` | **`00`** |
| **Only 1 person has an apple** (1 + 0 + 0) | **1** | `0` | `1` | **`01`** |
| **Any 2 people have an apple** (1 + 1 + 0) | **2** | `1` | `0` | **`10`** (1 two-pack + 0 singles) |
| **All 3 people have an apple** (1 + 1 + 1) | **3** | `1` | `1` | **`11`** (1 two-pack + 1 single) |

---

#### 🤖 How We Build It: The "Two Helper" Trick (With Real Values!)

Imagine you have two helpers (**Half Adder 1** and **Half Adder 2**).
Each helper is only smart enough to add **two numbers at a time** — it gives back a Sum bit and a Carry bit.

We will use the hardest case as our worked example:

```
   Friend A    = 1
   Friend B    = 1
   Carry In    = 1
   ─────────────────
   REAL ANSWER = 3  (which is "11" in binary: Carry Out = 1, Sum = 1)
```

Now watch how the two helpers figure this out together:

---

##### 🔵 HELPER 1 (Half Adder 1): Adds Friend A + Friend B

```
   Friend A = 1
   Friend B = 1

   Step 1a — The XOR Gate asks: "Are the inputs DIFFERENT?"
             1 XOR 1 = 0   (They are the SAME, so output is 0)
             ──► Temp Sum = 0

   Step 1b — The AND Gate asks: "Are BOTH inputs 1?"
             1 AND 1 = 1   (YES! Both are 1)
             ──► Temp Carry 1 = 1

   Helper 1 hands back:  Temp Sum = 0,  Temp Carry 1 = 1
```

So after Step 1, we know: "I added 1 + 1. The one's column is 0 and there IS a carry of 1."

---

##### 🟢 HELPER 2 (Half Adder 2): Adds Temp Sum + Carry In

> **💡 Why does Helper 2 receive `Temp Sum` and NOT `Temp Carry 1`?**
> Because of the **golden rule of math: You can only add numbers in the SAME column!**
> * **The One's Column (Value = 1):** Friend A, Friend B, Carry-In, and `Temp Sum`.
> * **The Two's Column (Value = 2):** Any Carry (`Temp Carry 1`, `Temp Carry 2`).
>
> When Helper 1 added `1 + 1`, the result was `2`. 
> - The **`Temp Sum = 0`** is what is left over in the **One's column**.
> - The **`Temp Carry 1 = 1`** was promoted to the **Two's column** (it represents a group of 2).
>
> When the 3rd friend arrives (`Carry In = 1`), they are holding a **single 1 unit**. They MUST combine their 1 unit with the remaining singles in the One's column (`Temp Sum = 0`). They cannot directly add a single 1 unit to a 2-group inside a 1-bit adder!

Helper 2 receives the Temp Sum (0) from Helper 1, and now adds the 3rd friend (Carry In = 1):

```
   Temp Sum = 0  (from Helper 1)
   Carry In = 1  (the 3rd friend arriving)

   Step 2a — The XOR Gate asks: "Are the inputs DIFFERENT?"
             0 XOR 1 = 1   (YES! They are different)
             ──► FINAL SUM = 1   ✅  (This is one output of the Full Adder!)

   Step 2b — The AND Gate asks: "Are BOTH inputs 1?"
             0 AND 1 = 0   (No, one of them is 0)
             ──► Temp Carry 2 = 0
```

---

##### 🔴 THE OR GATE (The Collector): Combines Both Carries

The OR gate now asks: "Did Helper 1 produce a carry OR did Helper 2 produce a carry?"

```
   Temp Carry 1 (from Helper 1) = 1
   Temp Carry 2 (from Helper 2) = 0

   1 OR 0 = 1

   ──► FINAL CARRY OUT = 1   ✅  (This is the second output of the Full Adder!)
```

---

##### ✅ Putting It All Together

```
   Inputs:   A = 1,  B = 1,  Carry In = 1

   Helper 1: Temp Sum = 0,  Temp Carry 1 = 1
   Helper 2: Final Sum = 1, Temp Carry 2 = 0
   OR Gate:  Final Carry Out = 1

   OUTPUT:   Carry Out = 1,  Sum = 1   →   Binary "11"  =  Decimal 3   ✅
```

This matches the apple table above: 3 people with 1 apple each = 3 apples total = `"11"` in binary!

---

#### 📌 Master Summary: The 3 Roles Inside a Full Adder

```
                  ┌─────────────────────────────────────────────────────────┐
                  │                       FULL ADDER                        │
                  │                                                         │
    Input A ─────►├──►[ HALF ADDER 1 ]──► Temp Sum ──►[ HALF ADDER 2 ]──────┼──► ⭐ FINAL SUM (1 bit)
    Input B ─────►├──►               ──► Temp Carry 1 ──┐                   │
                  │                                     ├──►[ OR GATE ]─────┼──► ⭐ FINAL CARRY OUT (1 bit)
    Carry In ────►├─────────────────────────────────────┘  (Combines both)  │
                  └─────────────────────────────────────────────────────────┘
```

| Component | What It Adds / Computes | What Output It Produces |
| :--- | :--- | :--- |
| **Half Adder 1 (Helper 1)** | Adds `Input A + Input B` | Produces `Temp Sum` (sent to Helper 2) and `Temp Carry 1` (sent to OR gate). |
| **Half Adder 2 (Helper 2)** | Adds `Temp Sum + Carry In` | Outputs the **⭐ FINAL SUM (1 bit)**, and sends `Temp Carry 2` to the OR gate. |
| **OR Gate (Collector)** | Checks `Temp Carry 1 OR Temp Carry 2` | Outputs the **⭐ FINAL CARRY OUT (1 bit)** to the next column on the left! |

---

#### 🔍 The Full Adder Wiring Diagram (All 5 Gates With Values Filled In)

```
                       ┌─────────┐
    A=1 ──────────────►│  XOR 1  ├──── Temp Sum = 0 ────────────────────┐
                       │  Gate   │                                       │
    B=1 ──────────────►│ 1 XOR 1 │                                       ▼
                       └─────────┘                                  ┌─────────┐
                       ┌─────────┐                                  │  XOR 2  ├──► FINAL SUM = 1
    A=1 ──────────────►│  AND 1  ├──── Temp Carry 1 = 1 ──────┐    │  Gate   │
                       │  Gate   │                             │    │ 0 XOR 1 │
    B=1 ──────────────►│ 1 AND 1 │                             │    └─────────┘
                       └─────────┘                             │
                                                               │    ┌─────────┐
    Temp Sum = 0 ─────────────────────────────────────────────►│    │  AND 2  ├──── Temp Carry 2 = 0
    Carry In = 1 ─────────────────────────────────────────────►│    │  Gate   │
                                                               │    │ 0 AND 1 │
                                                               │    └────┬────┘
                                                               │         │
                                                               ▼         ▼
                                                         ┌─────────────────┐
                                                         │    OR GATE      ├──► FINAL CARRY OUT = 1
                                                         │   1  OR  0 = 1  │
                                                         └─────────────────┘
```

* **2 XOR Gates:** Figure out the single-digit sum.
* **2 AND Gates:** Detect whether any pairs produced an overflow carry.
* **1 OR Gate:** If ANY carry was produced anywhere, pass it on.
* **Total Components:** **5 Gates = 28 MOSFET Transistors**.

---

### Step 2.3: Level 3 — The 4-Bit Ripple Carry Adder (With a Worked Example!)

To add two 4-bit numbers, we chain **four Full Adders side-by-side**. The key insight is the name: **Ripple Carry** — the carry from each column literally ripples leftwards, one column at a time, just like carrying on paper.

**Let us add 5 + 3 = 8:**

```
   A = 5  →  binary "0101"   (A[3]=0, A[2]=1, A[1]=0, A[0]=1)
   B = 3  →  binary "0011"   (B[3]=0, B[2]=0, B[1]=1, B[0]=1)
   ─────────────────────────
       8  →  binary "1000"   Expected answer
```

Watch each Full Adder work column-by-column from **right to left** (exactly like on paper):

```
   Cin=0 fixed ──────────────────────────────────────────────────────────────────────────────────────────────┐
                                                                                                              │
         ┌──────────────────────┐                                                                             │
         │  FULL ADDER — Bit 0  │◄─────────────────────────────────────────────────────────── Cin = 0         │
         │  A[0]=1   B[0]=1     │                                                                             │
         │                      │   Bit 0: 1 + 1 + 0 = 2  →  Sum[0] = 0,  Carry C1 = 1                       │
         └──────────┬───────────┘                                                                             │
                    │ C1=1                                                                                     │
                    ▼                                                                                          │
         ┌──────────────────────┐                                                                             │
         │  FULL ADDER — Bit 1  │◄──────────────────────────────────────── Cin = C1 = 1                       │
         │  A[1]=0   B[1]=1     │                                                                             │
         │                      │   Bit 1: 0 + 1 + 1 = 2  →  Sum[1] = 0,  Carry C2 = 1                       │
         └──────────┬───────────┘                                                                             │
                    │ C2=1                                                                                     │
                    ▼                                                                                          │
         ┌──────────────────────┐                                                                             │
         │  FULL ADDER — Bit 2  │◄──────────────────────────────────────── Cin = C2 = 1                       │
         │  A[2]=1   B[2]=0     │                                                                             │
         │                      │   Bit 2: 1 + 0 + 1 = 2  →  Sum[2] = 0,  Carry C3 = 1                       │
         └──────────┬───────────┘                                                                             │
                    │ C3=1                                                                                     │
                    ▼                                                                                          │
         ┌──────────────────────┐                                                                             │
         │  FULL ADDER — Bit 3  │◄──────────────────────────────────────── Cin = C3 = 1                       │
         │  A[3]=0   B[3]=0     │                                                                             │
         │                      │   Bit 3: 0 + 0 + 1 = 1  →  Sum[3] = 1,  Carry C4 = 0                       │
         └──────────────────────┘

   FINAL RESULT: Sum[3] Sum[2] Sum[1] Sum[0] = "1000" = Decimal 8   ✅
```

The carry "rippled" through every column — each Full Adder passed its carry to the next one, exactly like writing a 1 in the next column when doing long addition on paper!

* **Grand Total for 4-Bit Adder:**
  * 8 XOR Gates + 8 AND Gates + 4 OR Gates = **20 Logic Gates = ~112 Transistors**.

---


## 🔀 3. The 4-Bit Multiplexer (Deep Breakdown)

The **Multiplexer (MUX)** is the decision-making switchboard of the chip. In [`counter.v`](./lab_verilator/counter.v), it decides between:
* **Choice 0:** Normal increment value (`count + 1`).
* **Choice 1:** Reset value (`0000`).

---

### Step 3.1: Level 1 — The 1-Bit 2-to-1 MUX (With Real Values!)

A 1-Bit MUX has **one job**: given two candidate wires (A and B) and a Select wire (S), output only one of them.

```
Output = (A AND NOT S) OR (B AND S)
```

It is made of 4 gates: **1 NOT + 2 AND + 1 OR**.

```
                       ┌─────────┐
    Input A ──────────►│  AND 1  ├────────┐
                       │  Gate   │        │
    Select (S) ──o────►│         │        ▼
               (NOT)   └─────────┘    ┌────────┐
                                      │   OR   ├──────► Output (Y)
                       ┌─────────┐ ┌─►│  Gate  │
    Input B ──────────►│  AND 2  │ │  └────────┘
                       │  Gate   │─┘
    Select (S) ───────►│         │
                       └─────────┘
```

---

#### 🔵 Case 1: Select = 0 (Pass Input A through)

Let's say `Input A = 1`, `Input B = 0`, `Select (S) = 0`.

```
   NOT Gate:   NOT(S)  = NOT(0) = 1   (Inverted select)

   AND Gate 1: A AND NOT(S)  =  1 AND 1  =  1   ──► A is UNBLOCKED!
   AND Gate 2: B AND S       =  0 AND 0  =  0   ──► B is BLOCKED!

   OR Gate:    1 OR 0 = 1

   OUTPUT = 1  (Input A passed through)   ✅
```

---

#### 🟢 Case 2: Select = 1 (Pass Input B through)

Same inputs: `Input A = 1`, `Input B = 0`, but now `Select (S) = 1`.

```
   NOT Gate:   NOT(S)  = NOT(1) = 0   (Inverted select)

   AND Gate 1: A AND NOT(S)  =  1 AND 0  =  0   ──► A is BLOCKED!
   AND Gate 2: B AND S       =  0 AND 1  =  0   ──► B is UNBLOCKED (but B itself is 0)

   OR Gate:    0 OR 0 = 0

   OUTPUT = 0  (Input B passed through)   ✅
```

> Notice: AND Gate 2 "unblocked" Input B's path, but because B itself was `0`, the output is `0`.
> The MUX is NOT forcing the output to `1` or `0` — it is just choosing **which input to believe**.

---

#### 📌 1-Bit MUX Summary

| Select (S) | AND Gate 1 Result | AND Gate 2 Result | OR Output | Which Input Passed? |
| :---: | :---: | :---: | :---: | :---: |
| `0` | `A AND 1 = A` | `B AND 0 = 0` | `A OR 0 = A` | **Input A** |
| `1` | `A AND 0 = 0` | `B AND 1 = B` | `0 OR B = B` | **Input B** |

* **Components per 1-bit MUX:** 1 NOT + 2 AND + 1 OR = **18 Transistors**.

---

### Step 3.2: Level 2 — The 4-Bit 2-to-1 Multiplexer (With Real Values!)

A 4-Bit MUX is just **four 1-Bit MUX slices working in parallel**, all sharing the same ONE Select wire.

**The Worked Example: Counter has just reached `0101` (decimal 5). The Adder produced `0110` (decimal 6). Reset is not pressed (reset = 0).**

```
   Input A (from Adder):  0110   (count + 1 = 6)
   Input B (hardwired):   0000   (the reset value)
   Select (S = reset):    0      (reset NOT pressed)

   Question: Which 4-bit value passes to the Flip-Flops?
```

Here is how each of the 4 1-Bit MUX slices handles ONE bit at a time:

```
   Select Wire (S) = 0
        │
   [ NOT Gate ]  →  NOT(0) = 1  (Shared: only one NOT gate for all 4 slices!)
    │          │
  (NOT S = 1) (S = 0)
    │          │

   Bit 3:  A[3]=0, B[3]=0  →  (0 AND 1) OR (0 AND 0)  =  0 OR 0  =  Y[3] = 0
   Bit 2:  A[2]=1, B[2]=0  →  (1 AND 1) OR (0 AND 0)  =  1 OR 0  =  Y[2] = 1
   Bit 1:  A[1]=1, B[1]=0  →  (1 AND 1) OR (0 AND 0)  =  1 OR 0  =  Y[1] = 1
   Bit 0:  A[0]=0, B[0]=0  →  (0 AND 1) OR (0 AND 0)  =  0 OR 0  =  Y[0] = 0

   OUTPUT: Y[3] Y[2] Y[1] Y[0] = 0110 = Decimal 6   ✅
   (The Adder's result passed through — the counter will advance to 6!)
```

Now what if the user presses Reset (Select S = 1)?

```
   Select Wire (S) = 1
   NOT(1) = 0

   Bit 3:  A[3]=0, B[3]=0  →  (0 AND 0) OR (0 AND 1)  =  0 OR 0  =  Y[3] = 0
   Bit 2:  A[2]=1, B[2]=0  →  (1 AND 0) OR (0 AND 1)  =  0 OR 0  =  Y[2] = 0
   Bit 1:  A[1]=1, B[1]=0  →  (1 AND 0) OR (0 AND 1)  =  0 OR 0  =  Y[1] = 0
   Bit 0:  A[0]=0, B[0]=0  →  (0 AND 0) OR (0 AND 1)  =  0 OR 0  =  Y[0] = 0

   OUTPUT: Y[3] Y[2] Y[1] Y[0] = 0000 = Decimal 0   ✅
   (The reset value passed through — the counter resets to 0!)
```

* **Grand Total for 4-Bit MUX:**
  * 1 NOT Gate + 8 AND Gates + 4 OR Gates = **13 Logic Gates = ~74 Transistors**.

---

### 📏 The Simple Rule: MUX Inputs and Outputs

| MUX Type | Input Wires | Select Wires | Output Wires |
| :--- | :---: | :---: | :---: |
| **1-Bit 2-to-1 MUX** | 2 (one per choice) | 1 | **1** |
| **4-Bit 2-to-1 MUX** | 8 (four per choice) | 1 | **4** |
| **8-Bit 2-to-1 MUX** | 16 (eight per choice) | 1 | **8** |

> **The output always has the same number of wires as the bit-width!**
> A MUX never compresses or expands data — it just **chooses** which set of wires to pass through.

---

### 🔀 Where MUXes Are Used in a Full CPU (ARM7)

In `counter.v`, the MUX only has one job: choose between `count + 1` and `0000` (reset). But in a full CPU the exact same MUX circuit is used everywhere to make routing decisions based on instructions:

| Chip | MUX is choosing between... |
| :--- | :--- |
| **`counter.v`** | `count + 1` (normal) vs `0000` (reset) |
| **CPU Program Counter** | `PC + 4` (next instruction) vs jump address (branch instruction) |
| **CPU ALU Input** | Register value vs Immediate constant from instruction |
| **CPU Write-Back** | ALU result vs RAM value (which one gets saved into a Register) |
| **CPU Register File** | Which of the 16 registers (`R0`-`R15`) to read from |

---

### 💡 The Key Insight

A MUX doesn't know or care **what** the values mean. It only ever answers one question:

> **"Which of these inputs should I connect to the output right now?"**

* In `counter.v` that question is: *"Should I count or reset?"*
* In a CPU that question might be: *"Should I jump to a new address or keep going to the next instruction?"*

---

## 💾 4. What is a Register? (Memory Storage)

A **Register** is simply a group of **D Flip-Flops** working side-by-side to store an N-bit number.

* A **4-Bit Register** contains **4 D Flip-Flops** (FF3, FF2, FF1, FF0).
* A **32-Bit CPU Register** contains **32 D Flip-Flops**.

Think of a single D Flip-Flop like a **snapshot camera shutter**:
* Between clock ticks: the camera viewfinder shows the outside world, but the PHOTO has not been taken yet.
* On the rising clock edge: **CLICK!** The shutter fires — the current value is permanently captured.
* After the tick: the camera holds the old photo frozen, no matter what changes outside.

---

### The 3 Control Pins and What They Physically Do:

| Control Pin | What it connects to physically | What it does in hardware |
| :--- | :--- | :--- |
| **Clock (`clk`)** | Connected to the clock pin of **all 4 Flip-Flops**. | The universal heartbeat. On the rising edge (0 -> 1), all 4 Flip-Flops snap their doors shut, capturing the new 4-bit value simultaneously. |
| **Reset (`reset`)** | Connected to the **Select wire (S) of the 4-Bit MUX**. | When `reset = 1`, it forces the MUX to select `4'b0000`, draining all 4 Flip-Flops to zero on the next clock tick. |
| **Enable (`enable`)** | Connected to a second MUX stage. | When `enable = 0`, the MUX routes the **old count back into the Flip-Flops**, freezing the counter in place. |

---

### What Happens Inside a Flip-Flop on a Clock Tick (Worked Example!)

**The Scenario:** The counter currently holds `0101` (decimal 5). The MUX has chosen the value `0110` (decimal 6) to write in. The clock ticks!

```
   BEFORE THE CLOCK TICK:
   ┌──────────────────────────────────────────────────────┐
   │         4-BIT REGISTER (holding "0101")              │
   │  ┌───────┐  ┌───────┐  ┌───────┐  ┌───────┐          │
   │  │  FF3  │  │  FF2  │  │  FF1  │  │  FF0  │          │
   │  │  Q=0  │  │  Q=1  │  │  Q=0  │  │  Q=1  │          │
   │  │  D=0  │  │  D=1  │  │  D=1  │  │  D=0  │          │
   │  └───────┘  └───────┘  └───────┘  └───────┘          │
   │  (D input: next value "0110" is waiting at the door!) │
   └──────────────────────────────────────────────────────┘

   ⚡ CLOCK RISING EDGE FIRES! (0V → 5V on the clock wire)

   AFTER THE CLOCK TICK:
   ┌──────────────────────────────────────────────────────┐
   │         4-BIT REGISTER (now holds "0110")            │
   │  ┌───────┐  ┌───────┐  ┌───────┐  ┌───────┐          │
   │  │  FF3  │  │  FF2  │  │  FF1  │  │  FF0  │          │
   │  │  Q=0  │  │  Q=1  │  │  Q=1  │  │  Q=0  │          │
   │  └───────┘  └───────┘  └───────┘  └───────┘          │
   │  The snapshot was taken! Q now equals what D was.     │
   └──────────────────────────────────────────────────────┘
```

Each Flip-Flop independently:
* **FF3:** D was 0 → Q becomes **0** (no change)
* **FF2:** D was 1 → Q becomes **1** (no change)
* **FF1:** D was 1 → Q becomes **1** ⬅ changed from 0!
* **FF0:** D was 0 → Q becomes **0** ⬅ changed from 1!

All four happen **at the exact same nanosecond**. No Flip-Flop is "first" — they all fire simultaneously when the clock wire rises.

---

## 🚀 5. The Master Assembly: How It All Unites into `counter.v`

Here is the complete architectural blueprint showing all components working together:

```
               ┌──────────────────────────────────────────────┐
               │              4-BIT REGISTER                  │
               │   ┌───────┐  ┌───────┐  ┌───────┐  ┌───────┐ │
               │   │  FF3  │  │  FF2  │  │  FF1  │  │  FF0  │ │
               │   └───┬───┘  └───┬───┘  └───┬───┘  └───┬───┘ │
               └───────┼──────────┼──────────┼──────────┼─────┘
                       │ Q[3]     │ Q[2]     │ Q[1]     │ Q[0]
                       ├──────────┼──────────┼──────────┼────────► [ CHIP OUTPUT PINS ]
                       │          │          │          │          count[3:0]
                       ▼          ▼          ▼          ▼
               ┌──────────────────────────────────────────────┐
               │                 4-BIT ADDER                  │
               │  Input A: Current Q[3:0]                     │
               │  Input B: 0001 (+1)                          │
               └──────────────────────┬───────────────────────┘
                                      │ (Sum: Q + 1)
                                      ▼
               ┌──────────────────────────────────────────────┐
               │             4-BIT MULTIPLEXER                │◄── Reset Wire (Select)
               │                                              │◄── Enable Wire
               │  • If reset=1  ──► Route 0000                │
               │  • If enable=1 ──► Route Q + 1 (from Adder)  │
               │  • If enable=0 ──► Route old Q (Freeze)      │
               └──────────────────────┬───────────────────────┘
                                      │ (Next 4 bits waiting at D inputs)
                                      ▼
                        [ Loops back into Flip-Flops ]
```

---

### 🔄 The Complete Loop: One Full Cycle Traced With Real Values

Let's trace exactly what happens in ONE clock cycle when the counter holds `0101` (decimal 5).

```
STEP 1: THE REGISTER OUTPUTS ITS CURRENT VALUE
─────────────────────────────────────────────────────────────────
  The 4 Flip-Flops are holding "0101" (decimal 5).
  Their Q output pins are outputting voltage on 4 wires:
    FF3.Q = 0 (0V),  FF2.Q = 1 (5V),  FF1.Q = 0 (0V),  FF0.Q = 1 (5V)
  These 4 wires feed simultaneously into:
    → The chip's OUTPUT PINS (so the world can see count = 5)
    → The 4-BIT ADDER's Input A


STEP 2: THE ADDER COMPUTES count + 1
─────────────────────────────────────────────────────────────────
  Input A (from Register):  0101  (decimal 5)
  Input B (hardwired):      0001  (decimal 1)

  The 4 Full Adders work right-to-left (ripple carry):
    Bit 0:  1 + 1 + 0  =  Sum=0, Carry C1=1
    Bit 1:  0 + 0 + 1  =  Sum=1, Carry C2=0
    Bit 2:  1 + 0 + 0  =  Sum=1, Carry C3=0
    Bit 3:  0 + 0 + 0  =  Sum=0, Carry C4=0

  Adder Output:  0110  (decimal 6)   ✅
  This result travels down 4 wires into the MUX's Input A.


STEP 3: THE MUX DECIDES WHAT TO PASS TO THE FLIP-FLOPS
─────────────────────────────────────────────────────────────────
  MUX Input A (from Adder):  0110  (count + 1 = 6)
  MUX Input B (hardwired):   0000  (reset value)
  Select wire (reset pin):   0     (reset NOT pressed)

  Since S = 0 → Input A passes through!

  4-Bit MUX output: 0110  (decimal 6)
  This travels down 4 wires into the D (Data) pins of the Flip-Flops.


STEP 4: THE CLOCK FIRES — FLIP-FLOPS CAPTURE THE NEW VALUE
─────────────────────────────────────────────────────────────────
  ⚡ CLK rises from 0V to 5V!

  All 4 Flip-Flops snap simultaneously:
    FF3: D=0 → Q becomes 0   (no change)
    FF2: D=1 → Q becomes 1   (no change)
    FF1: D=1 → Q becomes 1   ⬅ was 0!
    FF0: D=0 → Q becomes 0   ⬅ was 1!

  Register now holds: 0110  (decimal 6)   ✅


STEP 5: LOOP RESTARTS IMMEDIATELY (no pause, no waiting!)
─────────────────────────────────────────────────────────────────
  The Q output pins now output "0110".
  The Adder is ALREADY computing 0110 + 0001 = 0111 (decimal 7).
  The MUX is ALREADY sitting with 0111 at its Input A.
  Everything is ready and waiting for the NEXT clock tick!
```

The whole loop (Steps 1-4) happens **in one single clock cycle** — often in under **1 nanosecond** on modern chips.

---

## 📊 Complete Transistor & Gate Tally for `counter.v`

| Sub-Circuit | Function | Logic Gates | Total MOSFET Transistors |
| :--- | :--- | :---: | :---: |
| **4-Bit Adder** | Computes `count + 1` | 20 Gates | **~112 Transistors** |
| **4-Bit MUX** | Handles `if (reset)` & `if (enable)` | 13 Gates | **~74 Transistors** |
| **4 D Flip-Flops** | Stores the 4-bit state (0 to 15) | 36 Gates | **~136 Transistors** |
| **TOTAL CHIP** | **Complete 4-Bit Synchronous Counter** | **69 Gates** | **~322 Transistors** |

---

## 🧭 Navigation
| ⬅️ Previous Guide | 🏠 Overview | ➡️ Next (Hands-on Labs) |
| :--- | :---: | ---: |
| [⬅️ 03. Hardware Emulation](./03_hardware_emulation.md) | [Section 1 Hub](./README.md) | [🔬 Verilator Lab ➡️](./lab_verilator/README.md) |

