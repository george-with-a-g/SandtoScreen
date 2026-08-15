# 01. Transistors to Logic Gates: The Physics of "1" and "0"

> *"If you want to understand modern computing from first principles, start with a single switch."*

---

## 1. The Real-World Analogy: The Electron Faucet

Imagine a plumbing pipe with water under pressure.
- If you place a valve in the middle, you can turn a knob to let water flow through, or turn it off to block the flow.
- A **Transistor** is simply a **water valve where the "knob" is controlled by electricity (voltage)** instead of your hand.

```
       [ Power Supply (VDD / 5V or 1.2V) ]  <-- High Pressure Water
                       |
                  [ Transistor ]            <-- Electrically controlled valve
                       |
              [ Output Wire (V_out) ]       <-- Where water/voltage flows
                       |
                  [ Transistor ]            <-- Another valve
                       |
               [ Ground (GND / 0V) ]        <-- The Drain / Zero Pressure
```

By applying a tiny voltage to the control pin (the **Gate**), we can allow or block the flow of current between the other two pins (**Source** and **Drain**).

---

## 2. The Two Building Blocks: NMOS and PMOS (CMOS)

Modern computer chips use **CMOS** (*Complementary Metal-Oxide-Semiconductor*).
CMOS chips use **two complementary types** of MOSFET transistors:

```
          NMOS Transistor                         PMOS Transistor
         (Pull-Down Switch)                     (Pull-Up Switch)

             Drain (D)                              Source (S)
                 |                                      |
       Gate (G)  |                            Gate (G)  o (Inverting Bubble)
       ----||----+                            ----||----+
                 |                                      |
             Source (S)                             Drain (D)

• Gate = 1 (HIGH Voltage) -> CLOSED (ON)    • Gate = 1 (HIGH Voltage) -> OPEN (OFF)
• Gate = 0 (LOW Voltage)  -> OPEN (OFF)     • Gate = 0 (LOW Voltage)  -> CLOSED (ON)
```

| Type | Gate = `0` (Low / 0V) | Gate = `1` (High / VDD) | Purpose in Circuit |
| :--- | :--- | :--- | :--- |
| **NMOS** | **OFF** (Disconnected) | **ON** (Conducts to GND) | Pulls the output down to `0` (Ground) |
| **PMOS** | **ON** (Conducts to VDD) | **OFF** (Disconnected) | Pulls the output up to `1` (VDD) |

---

## 3. Building the Simplest Gate: The CMOS Inverter (NOT Gate)

How do we invert a signal ($0 \to 1$ and $1 \to 0$)? We pair one PMOS on top with one NMOS on the bottom.

### Circuit Diagram
```
              VDD (1 / High)
                │
              ┌─┴─┐
              │ P │ (PMOS)
         IN ──┤ M ├──┐
              │ O │  │
              │ S │  │
              └─┬─┘  │
                │    ├─────── OUT
              ┌─┴─┐  │
              │ N │  │
         IN ──┤ M ├──┘
              │ O │
              │ S │ (NMOS)
              └─┬─┘
                │
              GND (0 / Low)
```

### How it works:
1. **When `IN = 0` (0V):**
   - The PMOS turns **ON** (connects `OUT` to $V_{DD}$).
   - The NMOS turns **OFF** (disconnects from $GND$).
   - **Result:** `OUT = 1` ($V_{DD}$).

2. **When `IN = 1` ($V_{DD}$):**
   - The PMOS turns **OFF** (disconnects from $V_{DD}$).
   - The NMOS turns **ON** (connects `OUT` to $GND$).
   - **Result:** `OUT = 0` ($GND$).

> **Why CMOS is brilliant:** In either state, one transistor is always OFF. There is **no direct path from $V_{DD}$ to $GND$**, meaning the circuit draws almost zero static power when not switching!

---

## 4. The Universal Building Block: The CMOS NAND Gate

A **NAND** gate outputs `0` only when **both** inputs $A$ and $B$ are `1`. In all other cases, it outputs `1`.

In CMOS:
- To pull `OUT` down to `0`, **both** NMOS transistors must be ON $\to$ **NMOS in Series**.
- To pull `OUT` up to `1`, **either** PMOS transistor can be ON $\to$ **PMOS in Parallel**.

```
                   VDD (1)
                  ┌───┴───┐
                  │       │
              ┌───┴───┐ ┌─┴───┐
              │ PMOS  │ │ PMOS│
         A ───┤   A   │ │  B  ├─── B
              └───┬───┘ └─┬───┘
                  └───┬───┘
                      ├───────────── OUT
                  ┌───┴───┐
                  │ NMOS  │
         A ───┤   A   │
                  └───┬───┘
                      │
                  ┌───┴───┐
                  │ NMOS  │
         B ───┤   B   │
                  └───┬───┘
                      │
                     GND (0)
```

### Truth Table for NAND:
| Input $A$ | Input $B$ | PMOS $A$ | PMOS $B$ | NMOS $A$ | NMOS $B$ | Output ($OUT$) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `0` | `0` | ON | ON | OFF | OFF | **`1`** ($V_{DD}$) |
| `0` | `1` | ON | OFF | OFF | ON | **`1`** ($V_{DD}$) |
| `1` | `0` | OFF | ON | ON | OFF | **`1`** ($V_{DD}$) |
| `1` | `1` | OFF | OFF | ON | ON | **`0`** ($GND$) |

---

## 5. Why NAND is Universal (Building Everything from NAND)

With NAND gates alone, you can create **any** logic gate in existence:

1. **NOT($A$):**
   $$\text{NOT}(A) = \text{NAND}(A, A)$$
2. **AND($A, B$):**
   $$\text{AND}(A, B) = \text{NOT}(\text{NAND}(A, B)) = \text{NAND}(\text{NAND}(A, B), \text{NAND}(A, B))$$
3. **OR($A, B$):** (By De Morgan's Law: $A \lor B = \overline{\overline{A} \land \overline{B}}$)
   $$\text{OR}(A, B) = \text{NAND}(\text{NOT}(A), \text{NOT}(B))$$
4. **XOR($A, B$):** (Exclusive OR, fundamental for binary addition)
   $$\text{XOR}(A, B) = \text{NAND}(\text{NAND}(A, \text{NAND}(A, B)), \text{NAND}(B, \text{NAND}(A, B)))$$

---

## 6. The Digital Abstraction: Fighting Physical Noise

Real physics is messy. Voltages fluctuate due to heat, wire resistance, and electromagnetic interference. 

How does a CPU run billions of operations per second without making math errors?
Through **Noise Margins**:

```
Voltage
 ↑  5.0V ─────── V_DD (Maximum Power)
 │  4.0V ─────── V_OH (Min High Output Voltage)
 │               [ LOGICAL 1 REGION ]
 │  3.0V ─────── V_IH (Min High Input Threshold)
 │  ─────────────────────────────────────────────
 │  2.5V         [ FORBIDDEN / UNDEFINED ZONE ]  <-- Circuit never rests here
 │  ─────────────────────────────────────────────
 │  1.5V ─────── V_IL (Max Low Input Threshold)
 │               [ LOGICAL 0 REGION ]
 │  0.5V ─────── V_OL (Max Low Output Voltage)
 0  0.0V ─────── GND (Ground)
```

Even if $0.5\text{V}$ of electrical noise is added to a line carrying a `0` ($0.2\text{V} \to 0.7\text{V}$), the receiving gate still interprets anything below $1.5\text{V}$ as an absolute, perfect boolean `0`. The analog noise is restored back to a clean digital signal at each gate stage!

---

## 7. Real World Context & Key Takeaway

* An **Apple M-series** or **Intel Core i9** chip contains over **20,000,000,000 to 100,000,000,000 MOSFET transistors**.
* In a fabricated chip (ASIC), these transistors are permanently etched onto a silicon wafer using ultraviolet photolithography.
* **The Problem:** Fabricating a silicon chip costs millions of dollars and takes months.
* **The Solution:** What if we had a chip whose transistors could be reconfigured in milliseconds to act like *any* circuit we want?
  $\implies$ **That is an FPGA!**

---

👉 Next Step: **[`02_fpgas_and_luts.md`](./02_fpgas_and_luts.md)** (How FPGAs and Look-Up Tables work)
