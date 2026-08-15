# 02. FPGAs and LUTs: Programmable Hardware Without Moving Silicon

> *"If an ASIC is a statue carved from stone, an FPGA is a box of Lego bricks that you can assemble, tear down, and rebuild in seconds."*

---

## 1. The Core Paradox

When a factory manufactures a microchip (like your laptop's CPU), physical metal wires and silicon transistors are permanently baked into place. You cannot add a wire or change a gate.

So how does a **Field-Programmable Gate Array (FPGA)** let you build custom processors, graphics pipelines, or network chips on a single physical chip that you can reconfigure on your desk?

**The secret:** An FPGA does not rewire transistors. Instead, it uses **Look-Up Tables (LUTs)** and **SRAM memory** to emulate *any* boolean function.

---

## 2. The Look-Up Table (LUT): Emulating Gates with Memory

Any digital logic gate is completely defined by its **Truth Table**.

For example, consider the function $Y = (A \land B) \lor C$:

| Input $A$ | Input $B$ | Input $C$ | $Y$ (Output) | SRAM Address / Bit |
| :---: | :---: | :---: | :---: | :---: |
| 0 | 0 | 0 | **0** | Bit 0 |
| 0 | 0 | 1 | **1** | Bit 1 |
| 0 | 1 | 0 | **0** | Bit 2 |
| 0 | 1 | 1 | **1** | Bit 3 |
| 1 | 0 | 0 | **0** | Bit 4 |
| 1 | 0 | 1 | **1** | Bit 5 |
| 1 | 1 | 0 | **1** | Bit 6 |
| 1 | 1 | 1 | **1** | Bit 7 |

### How a 3-Input LUT (LUT3) Implements This:
A LUT3 consists of:
1. **8 bits of static RAM (SRAM)** storing the column of outputs: `0b11101010`.
2. An **8-to-1 Multiplexer (MUX)** whose select lines are hooked up to inputs $A$, $B$, and $C$.

```
    Configuration Memory (SRAM)
    ┌───┬───┬───┬───┬───┬───┬───┬───┐
    │ 0 │ 1 │ 0 │ 1 │ 0 │ 1 │ 1 │ 1 │  (8 bits stored)
    └───┴───┴───┴───┴───┴───┴───┴───┘
      │   │   │   │   │   │   │   │
     ─┴───┴───┴───┴───┴───┴───┴───┴─
    \                              /
     \       8-to-1 Multiplexer   /
      \                          /
       ────────────┬─────────────
                   │
                   ▼  Output Y
               ┌───────┐
      Inputs   │ A B C │ (Selects which of the 8 bits reaches Y)
               └───────┘
```

When inputs change (e.g. $A=1, B=1, C=0$, which is address $6$), the multiplexer instantly routes the 6th SRAM bit (`1`) to the output pin.

> **Key Insight:** To change the hardware behavior of an FPGA, you don't etch new silicon. You just write new bits into the SRAM configuration cells (a **bitstream**)!

---

## 3. Adding Memory & Time: The D Flip-Flop

A LUT alone can only compute **Combinational Logic** (instantaneous outputs based on current inputs, without memory of the past).

To build processors and computers, we need **Sequential Logic** (storing state across clock cycles).

This is achieved with a **D Flip-Flop (D-FF)**:

```
              ┌─────────┐
    D (Data) ─┤ D     Q ├─ Q (Output: holds state)
              │         │
    Clock ───>│ >       │
              └─────────┘
```

* On every rising edge of the **Clock** ($\uparrow$): The value at input $D$ is captured and copied to output $Q$.
* Between clock ticks: The output $Q$ remains rock-solid and stable, even if $D$ fluctuates.

---

## 4. The Basic FPGA Building Block: The Logic Cell / CLB

FPGAs tile millions of identical blocks called **Configurable Logic Blocks (CLBs)** or **Logic Elements (LEs)** across the chip.

A single Logic Cell contains:
1. **A LUT (typically 4-input or 6-input)**: Computes boolean logic.
2. **A D Flip-Flop**: Stores the result on a clock edge if sequential logic is needed.
3. **A Bypass MUX**: Allows the designer to use the raw combinational output directly, or use the registered (clocked) output.
4. **Carry-Chain Logic**: Specialized fast silicon for high-speed arithmetic addition/subtraction.

```
                  ┌──────────────────────────────────────────────┐
                  │               FPGA LOGIC CELL                │
                  │                                              │
    Inputs A,B,C,D │   ┌────────┐                ┌──────────┐    │
    ─────────────>│───┤  LUT4  ├──┬─────────────┤0         │    │
                  │   └────────┘  │              │  BYPASS  ├───┼──> Combinational Out
                  │               │   ┌───────┐  │   MUX    │    │
                  │               └───┤D     Q├──┤1         │    │
                  │                   │       │  └────┬─────┘    │
    Clock ────────┼──────────────────>│ >     │       │          │
                  │                   └───────┘   Config Bit     │
                  └──────────────────────────────────────────────┘
```

---

## 5. Connecting the Dots: The Routing Matrix

Thousands of Logic Cells are arranged in a 2D grid. Between them sits a **Programmable Interconnect Matrix** consisting of horizontal/vertical wire channels and **Switch Boxes**:

```
      [ Logic Cell ] ─────── [ Switch Box ] ─────── [ Logic Cell ]
            │                      │                      │
            │                      │                      │
      [ Switch Box ] ─────── [ Switch Box ] ─────── [ Switch Box ]
            │                      │                      │
            │                      │                      │
      [ Logic Cell ] ─────── [ Switch Box ] ─────── [ Logic Cell ]
```

When you compile your design using FPGA software (Synthesis & Place-and-Route), the tools decide:
1. Which LUTs compute which logic formulas.
2. Which switch-box pass transistors are closed to physically connect the outputs of one cell to the inputs of another.

---

## 6. Real-World Use Cases: Why not just use a CPU?

| Domain | Why FPGAs are Used | Real-World Example |
| :--- | :--- | :--- |
| **High-Frequency Trading (HFT)** | Nanosecond reaction times; no operating system kernel or CPU interrupt jitter. | Processing market feeds and submitting orders in $< 50\text{ns}$. |
| **ASIC / CPU Prototyping** | Testing a new chip design at hardware speeds before spending \$50M on silicon masks. | Apple & Intel prototyping next-gen CPUs on FPGA racks. |
| **Aerospace & Defense** | Radiation-hardened reprogrammability; real-time parallel radar signal processing. | Mars rovers and satellite communication systems. |
| **5G / Telecom & Video** | Real-time 8K video encoding/decoding and beamforming that would choke a CPU. | Telecom base station transceivers. |

---

👉 Next Step: **[`03_hardware_emulation.md`](./03_hardware_emulation.md)** (Why and how we emulate hardware with Verilator on your laptop)
