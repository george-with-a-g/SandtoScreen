# 03. Finite State Machine (FSM) Design: The Brain of Hardware

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](./README.md) • [01. Thinking in Verilog](./01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](./02_blocking_vs_nonblocking.md) • [03. FSM Design](./03_fsm_design.md) • [💡 Lab 1: Blinky](./lab_blinky/README.md) • [📡 Lab 2: UART](./lab_uart/README.md)
---

Computers and digital chips are not just passive adders and wires — they need to **make decisions, follow sequences, and control communication protocols**.

The fundamental architecture used to build control logic in hardware is the **Finite State Machine (FSM)**.

Whether you are building a UART serial transmitter, an SD-card reader, an SPI bus, or a 5-stage RISC-V CPU pipeline, you will use an FSM.

---

## 🧠 The Anatomy of an FSM

Every hardware FSM consists of **three distinct physical sections**:

```
                       ┌────────────────────────────────────────────────────────┐
                       │               FINITE STATE MACHINE (FSM)               │
                       │                                                        │
    Input Signals ────►├──►[ NEXT-STATE LOGIC ]──► Next State ──►[ STATE REG ]──┼─┐
                       │     (Combinational)      (Wire bus)     (Flip-Flops)   │ │
                       │           ▲                                  │         │ │
                       │           │                                  │         │ │
                       │           └──────── Current State ───────────┴─────────┼─┼─► Current State
                       │                                                        │ │
                       │                                                        │ │
                       │   [   OUTPUT LOGIC   ]◄────────────────────────────────┘ │
                       │     (Combinational)                                      │
                       └───────────────────┬────────────────────────────────────┘
                                           ▼
                                     Output Signals
```

1. **State Register (Memory):** A set of D Flip-Flops that remember the **Current State** (e.g. `IDLE`, `START`, `DATA`, `STOP`).
2. **Next-State Logic (Brain):** Pure combinational logic that looks at the *Current State* and *Input Signals* and decides what the *Next State* should be.
3. **Output Logic (Action):** Pure combinational logic that turns control signals on or off based on which state the machine is currently in.

---

## 🚦 Moore vs. Mealy Machines

There are two primary styles of FSM:

| Feature | **Moore Machine** (Recommended) | **Mealy Machine** |
| :--- | :--- | :--- |
| **Output depends on...** | **Current State ONLY** | **Current State AND Current Inputs** |
| **Timing** | Output is stable and glitch-free | Output reacts instantly to input changes |
| **Best used for** | Control units, CPU decoders, protocol engines | High-speed handshaking, low-latency bridges |

> **Pro Tip:** In 90% of digital design, use a **Moore FSM**. Because outputs depend only on state flip-flops, they are completely immune to input noise and glitches.

---

## 🏗️ The Industry Standard 3-Block Verilog FSM Pattern

To write clean, bug-free FSMs in Verilog, professional engineers use the **Three-Block Pattern**:

### Example: A Serial Transmitter Controller (IDLE -> START -> DATA -> STOP)

```verilog
module fsm_transmitter (
    input  wire       clk,
    input  wire       reset,
    input  wire       start_trigger,
    output reg        tx_active,
    output reg        tx_pin
);

    // ========================================================================
    // 1. STATE ENCODING (Give human-readable names to binary numbers)
    // ========================================================================
    localparam STATE_IDLE  = 2'b00;
    localparam STATE_START = 2'b01;
    localparam STATE_DATA  = 2'b10;
    localparam STATE_STOP  = 2'b11;

    reg [1:0] current_state;
    reg [1:0] next_state;

    // ========================================================================
    // BLOCK 1: Clocked State Register (Sequential)
    // Moves current_state to next_state on each clock tick
    // ========================================================================
    always @(posedge clk) begin
        if (reset) begin
            current_state <= STATE_IDLE;
        end else begin
            current_state <= next_state;
        end
    end

    // ========================================================================
    // BLOCK 2: Next-State Logic (Combinational)
    // Decides WHERE we go next based on current_state and inputs
    // ========================================================================
    always @(*) begin
        // Default assignment prevents accidental latch synthesis!
        next_state = current_state;

        case (current_state)
            STATE_IDLE: begin
                if (start_trigger) next_state = STATE_START;
            end

            STATE_START: begin
                next_state = STATE_DATA;
            end

            STATE_DATA: begin
                next_state = STATE_STOP;
            end

            STATE_STOP: begin
                next_state = STATE_IDLE;
            end

            default: next_state = STATE_IDLE;
        endcase
    end

    // ========================================================================
    // BLOCK 3: Output Logic (Combinational Moore Style)
    // Decides WHAT signals to drive in each state
    // ========================================================================
    always @(*) begin
        case (current_state)
            STATE_IDLE: begin
                tx_active = 1'b0;
                tx_pin    = 1'b1; // Idle line is HIGH
            end

            STATE_START: begin
                tx_active = 1'b1;
                tx_pin    = 1'b0; // Start bit is LOW
            end

            STATE_DATA: begin
                tx_active = 1'b1;
                tx_pin    = 1'b1; // Sending data bit
            end

            STATE_STOP: begin
                tx_active = 1'b1;
                tx_pin    = 1'b1; // Stop bit is HIGH
            end

            default: begin
                tx_active = 1'b0;
                tx_pin    = 1'b1;
            end
        endcase
    end

endmodule
```

---

## 🛡️ Critical FSM Design Rules

1. **Always use a `default:` case in `case(...)` statements:**
   If a cosmic ray or electrical glitch puts your state register into an undefined state (e.g. `2'bxx`), the `default:` statement safely boots the machine back to `STATE_IDLE`.
2. **Assign default values at the top of Block 2:**
   Putting `next_state = current_state;` at the very top of `always @(*)` ensures every path has an assignment, preventing the synthesizer from accidentally creating unwanted transparent latches.
3. **Use `localparam` for state names:**
   Never use raw numbers like `if (state == 2)` in your logic. Always use descriptive names like `STATE_DATA`.

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 2 Hub | ➡️ Next Lab |
| :--- | :---: | ---: |
| [⬅️ 02. Blocking vs Non-Blocking](./02_blocking_vs_nonblocking.md) | [Section 2 Hub](./README.md) | [💡 Lab 1: Blinky LED ➡️](./lab_blinky/README.md) |
