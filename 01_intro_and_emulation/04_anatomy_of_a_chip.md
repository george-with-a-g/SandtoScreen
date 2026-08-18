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

### Step 2.2: Level 2 — The Full Adder: Explained Like You Are 8 Years Old

A **Half Adder** is great, but it is "half" because it can only add **two** numbers (A + B).

When you add big numbers on paper, you often have **THREE** numbers to add in a column:
1. Number A
2. Number B
3. The **Carry-In** from your neighbor on the right!

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

#### 🤖 How We Build It: The "Two Helper" Trick

Imagine you have two helpers (**Half Adder 1** and **Half Adder 2**). Each helper is only smart enough to add **two numbers at a time**.

How do you add all 3 friends using two helpers? **In 2 simple steps:**

```
                                  FULL ADDER
  ┌─────────────────────────────────────────────────────────────────────────┐
  │                                                                         │
  │   Friend A ──┐                                                          │
  │              ├──►[ HELPER 1 ]───► Temp Sum ──┐                          │
  │   Friend B ──┘   (Half Adder 1)              ├──►[ HELPER 2 ]───► FINAL │
  │                        │                     │   (Half Adder 2)   SUM   │
  │                        │ (Temp Carry 1)      │                          │
  │   Carry In ────────────┼─────────────────────┘                          │
  │                        │                                                │
  │                        ▼                                                │
  │                  ┌───────────┐ (Temp Carry 2)                           │
  │                  │  OR GATE  │◄─────────────────────────────────────────┤
  │                  └─────┬─────┘                                          │
  │                        ▼                                                │
  │                  FINAL CARRY OUT (Pass to neighbor on the left!)        │
  │                                                                         │
  └─────────────────────────────────────────────────────────────────────────┘
```

##### Step 1: Helper 1 (Half Adder 1)
* Adds **Friend A + Friend B**.
* Gives us a **Temporary Sum** and a **Temporary Carry 1**.

##### Step 2: Helper 2 (Half Adder 2)
* Takes that **Temporary Sum** and adds the 3rd person: **Carry-In**.
* Produces the **FINAL SUM**!

##### Step 3: The OR Gate (The Collector)
* Did Helper 1 get a carry **OR** did Helper 2 get a carry?
* If *either* helper produced a carry, the OR gate sends a **`1` to Carry Out**!
* *(Can both helpers produce a carry at the same time? No, because 3 people can never make 4 apples!)*

---

#### 🔍 Looking Under the Hood: The 5 Physical Gates

If you open up a Full Adder, you will see exactly **5 logic gates**:

```
                       ┌─────────┐
    Input A ──────────►│  XOR 1  ├───────┬───────┐
                       │  Gate   │       │       │
    Input B ──────────►│         │       │       ▼
                       └─────────┘       │  ┌─────────┐
                       ┌─────────┐       │  │  XOR 2  ├────────► FINAL SUM
    Input A ──────────►│  AND 1  │       └─►│  Gate   │
                       │  Gate   ├─┐        │         │
    Input B ──────────►│         │ │ CarryIn┼►│         │
                       └─────────┘ │        └─────────┘
                                   │        ┌─────────┐
                                   │        │  AND 2  ├─┐
                                   │        │  Gate   │ │
                                   │        └─────────┘ │
                                   │             ▲      │
                                   │ (Carry 1)   │      │ (Carry 2)
                                   ▼             │      ▼
                                ┌────────────────┴────────┐
                                │         OR GATE         ├────► FINAL CARRY OUT
                                └─────────────────────────┘
```

* **2 XOR Gates:** Compute the single-apple sum (A XOR B XOR CarryIn).
* **2 AND Gates:** Detect pairs of apples (Carries).
* **1 OR Gate:** Combines the carries.
* **Total Components:** **5 Gates = 28 MOSFET Transistors**.

---

### Step 2.3: Level 3 — The Complete 4-Bit Ripple Carry Adder
To add two 4-bit numbers (A[3:0] + B[3:0]), we chain **four 1-Bit Full Adders side-by-side**:

```
        Cout              C3               C2               C1            Cin = 0
         ▲                ▲                ▲                ▲                ▲
         │                │                │                │                │
   ┌─────┴────────┐  ┌────┴─────────┐  ┌───┴──────────┐  ┌──┴───────────┐    │
   │  FULL ADDER  │  │  FULL ADDER  │  │  FULL ADDER  │  │  FULL ADDER  │    │
   │    Bit 3     │◄─┤    Bit 2     │◄─┤    Bit 1     │◄─┤    Bit 0     │◄───┘
   │ A[3]    B[3] │  │ A[2]    B[2] │  │ A[1]    B[1] │  │ A[0]    B[0] │
   └─────┬────────┘  └────┬─────────┘  └───┬──────────┘  └───┬──────────┘
         │                │                │                 │
         ▼                ▼                ▼                 ▼
       Sum[3]           Sum[2]           Sum[1]            Sum[0]
  (Eight's place)   (Four's place)    (Two's place)     (One's place)
```

* **Grand Total for 4-Bit Adder:** 
  * 8 XOR Gates + 8 AND Gates + 4 OR Gates = **20 Logic Gates = ~112 Transistors**.

---

## 🔀 3. The 4-Bit Multiplexer (Deep Breakdown)

The **Multiplexer (MUX)** is the decision-making switchboard of the chip. In [`counter.v`](./lab_verilator/counter.v), it decides between:
* **Choice 0:** Normal increment value (`count + 1`).
* **Choice 1:** Reset value (`0000`).

---

### Step 3.1: Level 1 — The 1-Bit 2-to-1 MUX
Selects between 1-bit Input A and 1-bit Input B using a Select wire (S):
```text
Output = (A AND NOT S) OR (B AND S)
```

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
* If S = 0: `AND 1` is enabled -> Output is **Input A**.
* If S = 1: `AND 2` is enabled -> Output is **Input B**.
* **Components per 1-bit MUX:** 2 AND + 1 OR (+ 1 shared NOT) = **18 Transistors**.

---

### Step 3.2: Level 2 — The 4-Bit 2-to-1 Multiplexer
To switch a 4-bit bus (A[3:0] vs B[3:0]), we align **four 1-Bit MUX slices** that all share the exact same Select wire (S):

```
                            Select Wire (S)
                                 │
                            [ NOT Gate ] (Shared Inverter: 2 Transistors)
                             │        │
                     (Inverted S)   (Raw S)
                             │        │
   ┌─────────────────────────┼────────┼─────────────────────────┐
   │ Bit 3:  2 AND + 1 OR ◄──┴────────┘  (Output Y[3])          │
   │ Bit 2:  2 AND + 1 OR ◄──┴────────┘  (Output Y[2])          │
   │ Bit 1:  2 AND + 1 OR ◄──┴────────┘  (Output Y[1])          │
   │ Bit 0:  2 AND + 1 OR ◄──┴────────┘  (Output Y[0])          │
   └────────────────────────────────────────────────────────────┘
```

* **Grand Total for 4-Bit MUX:** 
  * 1 NOT Gate + 8 AND Gates + 4 OR Gates = **13 Logic Gates = ~74 Transistors**.

---

## 💾 4. What is a Register? (Memory Storage)

A **Register** is simply a group of **D Flip-Flops** working side-by-side to store an N-bit number.

* A **4-Bit Register** contains **4 D Flip-Flops** (FF3, FF2, FF1, FF0).
* A **32-Bit CPU Register** contains **32 D Flip-Flops**.

---

### The 3 Control Pins and What They Physically Do:

| Control Pin | What it connects to physically | What it does in hardware |
| :--- | :--- | :--- |
| **Clock (`clk`)** | Connected to the clock pin of **all 4 Flip-Flops**. | The universal heartbeat. On the rising edge (0 -> 1), all 4 Flip-Flops snap their doors shut, capturing the new 4-bit value simultaneously. |
| **Reset (`reset`)** | Connected to the **Select wire (S) of the 4-Bit MUX**. | When `reset = 1`, it forces the MUX to select `4'b0000`, draining all 4 Flip-Flops to zero on the next clock tick. |
| **Enable (`enable`)** | Connected to a second MUX stage. | When `enable = 0`, the MUX routes the **old count back into the Flip-Flops**, freezing the counter in place. |

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

