# Lab 1: Blinking an LED & Clock Prescalers

---
### 🧭 Section 2 Quick Links
[🏠 Section 2 Hub](../README.md) • [01. Thinking in Verilog](../01_what_is_verilog.md) • [02. Blocking vs Non-Blocking](../02_blocking_vs_nonblocking.md) • [03. FSM Design](../03_fsm_design.md) • [💡 Lab 1: Blinky](./README.md) • [📡 Lab 2: UART](../lab_uart/README.md)
---

Blinking an LED is the classic "Hello World" of digital hardware design.

In software, blinking an LED is usually just `sleep(1)`. But in digital hardware, there is no OS and no `sleep()` function. There is only a quartz crystal oscillator ticking at **50,000,000 times per second (50 MHz)**!

To blink an LED at 1 Hz (once per second), your hardware must **count clock ticks** to measure time.

---

## 🧮 The Clock Prescaler Math

A crystal oscillator produces a continuous 50 MHz square wave:
* 1 complete clock cycle = 20 nanoseconds (0.00000002 seconds).
* To toggle an LED on/off once per second (1 Hz total blink rate):
  * LED stays **ON** for 0.5 seconds = 25,000,000 clock cycles.
  * LED stays **OFF** for 0.5 seconds = 25,000,000 clock cycles.

```
       50 MHz Clock:   ┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐ (Ticking 50M times/sec!)
                       └┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘
                                       │
                                       ▼ [ Prescaler Counter (25-bit) ]
                                       │
       1 Hz LED Blink: ┌───────────────────────────────┐
                       │   ON (25 Million cycles)      │   OFF (25 Million cycles)
                       └───────────────────────────────┴───────────────────────
```

### How many flip-flops are needed to count to 25,000,000?
Since 2^24 = 16,777,216 and 2^25 = 33,554,432, we need a **25-bit counter** (25 D Flip-Flops chained together).

---

## 📁 Lab Files

| File | Description |
| :--- | :--- |
| [`blinky.v`](./blinky.v) | The hardware module with parameterized `TOGGLE_LIMIT`. |
| [`sim_main.cpp`](./sim_main.cpp) | The Verilator C++ testbench generating clock signals and dumping `.vcd` traces. |
| [`Makefile`](./Makefile) | Automation script for Verilator compilation and GTKWave launching. |

---

## 🏃 How to Run the Lab

Compile and simulate in Verilator with one command:

```bash
make run
```

### Expected Output:
```text
==============================================================
           RUNNING BLINKY VERILATOR SIMULATION                
==============================================================

Time (ns)   Clock   Reset     LED   Event Description
-------------------------------------------------------
      10       1       1       0   Reset active (LED held LOW)
      20       1       1       0   Reset active (LED held LOW)
      30       1       1       0   Reset active (LED held LOW)
      40       1       1       0   Reset active (LED held LOW)
      50       1       0       0   
     ...
     140       1       0       1   ⚡ LED TOGGLED ON  [HIGH]
     ...
     240       1       0       0   🌑 LED TOGGLED OFF [LOW]
     ...
     340       1       0       1   ⚡ LED TOGGLED ON  [HIGH]
-------------------------------------------------------

✅ Simulation complete! Waveform saved to blinky.vcd
```

---

## 🔍 Visualizing the Waveform in GTKWave

To inspect the clock cycles and LED toggle edge visually:

```bash
make view
```

*(Or launch manually: `gtkwave blinky.vcd`)*

In GTKWave:
1. Expand the `TOP` hierarchy on the left panel.
2. Select `blinky`.
3. Drag `clk`, `reset`, `counter[3:0]`, and `led` into the Signals window.
4. Click the **Zoom Fit** button (or press `Ctrl + Shift + F`) to see the LED toggle square wave!

---

## 🧭 Navigation
| ⬅️ Previous Chapter | 🏠 Section 2 Hub | ➡️ Next Lab |
| :--- | :---: | ---: |
| [⬅️ 03. FSM Design](../03_fsm_design.md) | [Section 2 Hub](../README.md) | [📡 Lab 2: UART Controller ➡️](../lab_uart/README.md) |
