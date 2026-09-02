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

## 🔍 Line-by-Line Breakdown of the Serial Transmitter Controller

Let's dissect what every section of this hardware module physically does:

### 1. 🔌 Module Pins (The Metal Leads)
* **`input wire clk`**: The heartbeat quartz clock driving the state flip-flops.
* **`input wire reset`**: Active-high signal that forces the controller back to `STATE_IDLE`.
* **`input wire start_trigger`**: A pulse from the rest of the chip saying: *"I have data, start transmitting now!"*
* **`output reg tx_active`**: A status flag (1 = Busy transmitting, 0 = Ready for a new command).
* **`output reg tx_pin`**: The single physical serial copper wire connecting to the outside world.

---

### 2. 🏷️ State Names (`localparam`)
```verilog
localparam STATE_IDLE  = 2'b00;
localparam STATE_START = 2'b01;
localparam STATE_DATA  = 2'b10;
localparam STATE_STOP  = 2'b11;
```
* Since we have **4 states**, we need **2 bits** of storage (2^2 = 4): `00`, `01`, `10`, `11`.
* `localparam` gives human-readable labels to these binary numbers so we never write confusing magic numbers in our code.
* **`current_state`**: The 2 physical D Flip-Flops holding where the machine is **right now**.
* **`next_state`**: The 2-bit combinational wire calculating where the machine will go **on the next clock tick**.

---

### 3. 💾 BLOCK 1: Clocked State Register (Memory)
```verilog
always @(posedge clk) begin
    if (reset) begin
        current_state <= STATE_IDLE;
    end else begin
        current_state <= next_state;
    end
end
```
* **This is the ONLY block in the entire FSM with physical memory.**
* On every rising clock edge (`posedge clk`), it copies `next_state` into `current_state`.
* If `reset == 1`, it resets `current_state` back to `STATE_IDLE` (`2'b00`).
* Uses **`<=` (Non-blocking)** because it builds clocked D Flip-Flops.

---

### 4. 🧠 BLOCK 2: Next-State Logic (The Brain)
```verilog
always @(*) begin
    next_state = current_state; // Safety default

    case (current_state)
        STATE_IDLE: begin
            if (start_trigger) next_state = STATE_START;
        end
        STATE_START: next_state = STATE_DATA;
        STATE_DATA:  next_state = STATE_STOP;
        STATE_STOP:  next_state = STATE_IDLE;
        default:     next_state = STATE_IDLE;
    endcase
end
```
* Pure combinational logic using **`always @(*)`** and **`=` (Blocking)**.
* Answers one question: *"Given where I am right now (`current_state`) and input pins (`start_trigger`), where should I go next (`next_state`)?"*
* **`next_state = current_state;`**: Assigned at the very top as a default fallback to prevent the synthesizer from accidentally creating unwanted transparent latches.

---

### 5. ⚡ BLOCK 3: Output Logic (The Hands & Actuators)
```verilog
always @(*) begin
    case (current_state)
        STATE_IDLE:  begin tx_active = 1'b0; tx_pin = 1'b1; end
        STATE_START: begin tx_active = 1'b1; tx_pin = 1'b0; end
        STATE_DATA:  begin tx_active = 1'b1; tx_pin = 1'b1; end
        STATE_STOP:  begin tx_active = 1'b1; tx_pin = 1'b1; end
        default:     begin tx_active = 1'b0; tx_pin = 1'b1; end
    endcase
end
```
* A **Moore Output Block**: looks *only* at `current_state` and decides what voltage levels to drive on the output pins.
* When in `STATE_IDLE`: line sits at `1` (5V / quiet) and `tx_active` is `0`.
* When in `STATE_START`: line drops to `0` (0V) to alert the receiver, and `tx_active` turns `1` (busy).

---

## 🎬 Walking Through One Full Transmission Cycle

Here is the exact step-by-step trace of how this hardware executes over 6 clock ticks:

| Clock Tick | Inputs | `current_state` | `next_state` | `tx_active` | `tx_pin` (Wire Voltage) | Physical Event Description |
| :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **Tick 0** | `start_trigger = 0` | `STATE_IDLE` | `STATE_IDLE` | `0` | **`1` (5V)** | Line is silent and waiting. |
| **Tick 1** | `start_trigger = 1` | `STATE_IDLE` | `STATE_START`| `0` | **`1` (5V)** | Trigger fired! `next_state` computes `STATE_START`. |
| **Tick 2** | `start_trigger = 0` | `STATE_START`| `STATE_DATA` | **`1`** | **`0` (0V)** | ⚡ **START BIT:** Line drops to 0V to wake up receiver! |
| **Tick 3** | `start_trigger = 0` | `STATE_DATA` | `STATE_STOP` | **`1`** | **`1` (5V)** | 📦 **DATA BIT:** Payload bit is driven onto the wire. |
| **Tick 4** | `start_trigger = 0` | `STATE_STOP` | `STATE_IDLE` | **`1`** | **`1` (5V)** | 🛑 **STOP BIT:** Line returns HIGH to close the frame. |
| **Tick 5** | `start_trigger = 0` | `STATE_IDLE` | `STATE_IDLE` | `0` | **`1` (5V)** | ✅ Transmission complete! Back in IDLE for next command. |

---

## 💡 Why Professional Engineers Use the 3-Block Pattern:

```
  ┌─────────────────────────┐     ┌─────────────────────────┐     ┌─────────────────────────┐
  │         BLOCK 1         │     │         BLOCK 2         │     │         BLOCK 3         │
  │     (State Register)    │     │    (Next-State Logic)   │     │      (Output Logic)     │
  ├─────────────────────────┤     ├─────────────────────────┤     ├─────────────────────────┤
  │ Handles TIME & CLOCK    │     │ Handles DECISIONS       │     │ Handles SIGNALS         │
  │ (posedge clk Flip-Flops)│     │ (Where do we go next?)  │     │ (What pins turn on/off?)│
  └─────────────────────────┘     └─────────────────────────┘     └─────────────────────────┘
```

Separating these 3 responsibilities makes complex hardware 100% predictable, completely eliminates simulation race conditions, and makes debugging waveform traces effortless!

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
