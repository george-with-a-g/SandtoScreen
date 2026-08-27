# Lab 2: Building a Real UART Serial Controller & Echo Server

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](../README.md) • [01. Thinking in Verilog](../01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](../02_blocking_vs_nonblocking.md) • [03. FSM Design](../03_fsm_design.md) • [💡 Lab 1: Blinky](../lab_blinky/README.md) • [📡 Lab 2: UART](./README.md)
---

The **UART (Universal Asynchronous Receiver-Transmitter)** is the most important bridge in computer engineering. It is how your microchip talks to your computer, prints debug messages to a terminal, and downloads boot code into memory.

In this lab, you will build a complete **bidirectional UART serial controller** in Verilog from scratch.

---

## 📡 The 8-N-1 Serial Protocol

UART is **asynchronous** — meaning there is **no shared clock wire** between the sender and receiver. They only share two single wires: **TX** (Transmit) and **RX** (Receive).

To send an 8-bit byte over one wire, UART packages the byte into an **8-N-1 Frame**:

```
              ┌────────────────────────────────────────────────────────┐
   IDLE (1)   │ START (0) │ D0 │ D1 │ D2 │ D3 │ D4 │ D5 │ D6 │ D7 │ STOP (1)│  IDLE (1)
 ─────────────┘           └───────────────────────────────────────┴─────────┴──────────
                          ◄──────── 8 Data Bits (LSB first) ──────►
```

1. **Idle Line (HIGH / 1):** When no data is being sent, the line sits at 5V (HIGH).
2. **Start Bit (LOW / 0):** The sender pulls the line LOW for 1 baud period to announce: *"A byte is coming!"*
3. **8 Data Bits:** The 8 bits of the byte are transmitted sequentially, **Least Significant Bit (LSB) first**.
4. **Stop Bit (HIGH / 1):** The line returns HIGH for 1 baud period to allow the receiver to settle and re-synchronize.

---

## ⏱️ Baud Rate & Clock Math

The **Baud Rate** is the number of bits transmitted per second. The most common standard speed is **115,200 baud**.

To measure bit duration, your hardware counts clock cycles:

```text
CLKS_PER_BIT = System Clock Frequency / Baud Rate

For a 50 MHz FPGA Clock @ 115,200 Baud:
CLKS_PER_BIT = 50,000,000 / 115,200 = 434 clock cycles per bit
```

Every 434 clock cycles, the transmitter shifts out the next bit!

---

## 🎯 The Receiver Trick: Mid-Bit Sampling

Because there is no shared clock, the receiver's oscillator and transmitter's oscillator might have small timing differences.

To achieve maximum noise immunity, the receiver uses **Mid-Bit Sampling**:

```
      Start Bit                  Data Bit 0                 Data Bit 1
 ┌─────────────────┐        ┌─────────────────┐        ┌─────────────────┐
 │                 │        │                 │        │                 │
 └─────────────────┴────────┴─────────────────┴────────┴─────────────────┘
          ▲                          ▲                          ▲
       Sample at                  Sample at                  Sample at
     CLKS_PER_BIT / 2           CLKS_PER_BIT               CLKS_PER_BIT
   (Center of Start)          (Center of Bit 0)          (Center of Bit 1)
```

1. When the falling edge (1 -> 0) is detected, the receiver counts to **`CLKS_PER_BIT / 2`** (the exact dead center of the start bit).
2. It verifies the line is still 0 (filtering out electrical noise spikes).
3. From that moment on, it waits a full **`CLKS_PER_BIT`** interval between samples — guaranteeing that every bit is sampled at the cleanest, most stable point in time!

---

## 📁 Lab Files Overview

| File | Description |
| :--- | :--- |
| [`uart_tx.v`](./uart_tx.v) | Hardware Serial Transmitter FSM (Serializes 8-bit bytes to TX pin). |
| [`uart_rx.v`](./uart_rx.v) | Hardware Serial Receiver FSM (Deserializes RX pin into 8-bit bytes with mid-bit sampling & metastability synchronizer). |
| [`uart_top.v`](./uart_top.v) | Top module uniting TX and RX into an **Echo Server** with command decoding (`'1'` / `'0'` LED toggle). |
| [`sim_main.cpp`](./sim_main.cpp) | Verilator C++ testbench acting as the virtual PC serial terminal. |
| [`Makefile`](./Makefile) | Automated build and waveform launch script. |

---

## 🏃 How to Run the Lab

Compile and simulate in Verilator with one command:

```bash
make run
```

### Expected Output:
```text
==============================================================
            RUNNING UART SERIAL ECHO TESTBENCH                
==============================================================

📡 Virtual Terminal transmitting string: "HELLO VERILOG! 1 0"

  -> Transmitted Byte: 'H' (ASCII 0x48)
  <- Hardware Echoed:  'H' | LED Status Pin: 0
  -> Transmitted Byte: 'E' (ASCII 0x45)
  <- Hardware Echoed:  'E' | LED Status Pin: 0
  -> Transmitted Byte: 'L' (ASCII 0x4c)
  <- Hardware Echoed:  'L' | LED Status Pin: 0
  -> Transmitted Byte: 'L' (ASCII 0x4c)
  <- Hardware Echoed:  'L' | LED Status Pin: 0
  -> Transmitted Byte: 'O' (ASCII 0x4f)
  <- Hardware Echoed:  'O' | LED Status Pin: 0
  ...
  -> Transmitted Byte: '1' (ASCII 0x31)
  <- Hardware Echoed:  '1' | LED Status Pin: 1   <-- ⚡ LED turned ON via UART!
  -> Transmitted Byte: '0' (ASCII 0x30)
  <- Hardware Echoed:  '0' | LED Status Pin: 0   <-- 🌑 LED turned OFF via UART!

--------------------------------------------------------------
Original Sent:   "HELLO VERILOG! 1 0"
Hardware Echoed: "HELLO VERILOG! 1 0"
--------------------------------------------------------------

🎉 SUCCESS! All bytes perfectly echoed through UART hardware!
   Waveform trace saved to uart.vcd (open with: gtkwave uart.vcd)
```

---

## 🔍 Visualizing the Serial Protocol in GTKWave

To visually see the start bits, data bits, and stop bits flowing across the wires:

```bash
make view
```

*(Or run: `gtkwave uart.vcd`)*

In GTKWave:
1. Select `uart_top`.
2. Add `clk`, `uart_rx_pin`, `uart_tx_pin`, `rx_byte[7:0]`, `rx_byte_valid`, and `led_status`.
3. Zoom in on any transmission to see the exact 8-N-1 serial framing pulse train!

---

## 🧭 Navigation
| ⬅️ Previous Lab | 🏠 Section 2 Hub | ➡️ Next Section |
| :--- | :---: | ---: |
| [⬅️ Lab 1: Blinky LED](../lab_blinky/README.md) | [Section 2 Hub](../README.md) | [Section 3: Building an ARM7 Processor ➡️](../../README.md) |
