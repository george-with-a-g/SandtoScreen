# Lab: Your First Verilator Hardware Emulation

In this lab, you compile a real **Verilog hardware module** into a **C++ simulation binary**, run cycle-by-cycle simulation, and produce a **`.vcd` waveform trace**.

---

## 📁 Lab Files

- [`counter.v`](./counter.v): The hardware description of a 4-bit synchronous binary counter.
- [`sim_main.cpp`](./sim_main.cpp): The C++ testbench that instantiates the hardware, drives clock/reset pins, and logs signals.
- [`Makefile`](./Makefile): Build automation script.

---

## 🛠️ Step 1: Install Verilator & GTKWave

If you have not already installed Verilator and GTKWave on Ubuntu / Debian / WSL2:
```bash
sudo apt update
sudo apt install -y verilator gtkwave
```

---

## 🚀 Step 2: Compile and Run Simulation

Run the following command in this directory:
```bash
make run
```

### What Verilator is doing behind the scenes:
1. `verilator --cc counter.v`: Parses `counter.v` and generates C++ classes inside `obj_dir/` (`Vcounter.h`, `Vcounter.cpp`).
2. Compiles `sim_main.cpp` together with the generated classes using `g++`.
3. Executes `./obj_dir/Vcounter`.
4. Outputs the simulation log to your terminal and dumps `counter.vcd`.

Expected terminal output:
```text
====================================================
       VERILATOR 4-BIT COUNTER SIMULATION           
====================================================
Time(ns)  CLK  RESET  ENABLE  COUNT(Hex)  COUNT(Dec)
----------------------------------------------------
      15    1     0      1        0x0        0
      25    1     0      1        0x1        1
      35    1     0      1        0x2        2
      45    1     0      1        0x3        3
      55    1     0      1        0x4        4
      65    1     0      1        0x5        5
      75    1     0      1        0x6        6
----------------------------------------------------
```

---

## 📊 Step 3: Inspect Waveforms in GTKWave

Launch GTKWave to visualize the digital signals:
```bash
make wave
# or: gtkwave counter.vcd
```

### In GTKWave:
1. In the **SST panel** (top left), click on `TOP` -> `counter`.
2. Select signals (`clk`, `reset`, `enable`, `count[3:0]`) and click **Append**.
3. Click the **Zoom Fit** icon (magnifying glass with square) on the top toolbar to see the full timeline!

---

## 💡 Practice Challenges

To solidify your understanding:
1. **Down Counter:** Change [`counter.v`](./counter.v) so that when a new input `down` is `1`, the counter decrements instead of increments.
2. **8-bit Counter:** Expand the counter width from `[3:0]` (0 to 15) to `[7:0]` (0 to 255).
3. **Mod-10 (Decade) Counter:** Make the counter roll over back to `0` whenever it reaches `9`.
