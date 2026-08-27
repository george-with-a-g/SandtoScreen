/**
 * lab_blinky/sim_main.cpp
 *
 * Verilator C++ simulation testbench for the Blinky module.
 * Generates clock cycles, handles reset, logs simulation progress,
 * and dumps digital waveform traces to blinky.vcd.
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vblinky.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // Instantiate the Verilated module
    auto top = std::make_unique<Vblinky>();

    // Enable waveform tracing
    Verilated::traceEverOn(true);
    auto tfp = std::make_unique<VerilatedVcdC>();
    top->trace(tfp.get(), 99); // Trace 99 levels of hierarchy
    tfp->open("blinky.vcd");

    std::cout << "==============================================================\n";
    std::cout << "           RUNNING BLINKY VERILATOR SIMULATION                \n";
    std::cout << "==============================================================\n\n";

    std::cout << std::setw(8) << "Time (ns)"
              << std::setw(8) << "Clock"
              << std::setw(8) << "Reset"
              << std::setw(8) << "LED"
              << "   Event Description\n";
    std::cout << std::string(55, '-') << "\n";

    vluint64_t sim_time = 0;
    int last_led = -1;

    // Initial state
    top->clk = 0;
    top->reset = 1; // Assert reset initially

    // Run simulation for 120 half-cycles (60 full clock periods)
    while (sim_time < 120) {
        // Toggle clock: 0 -> 1 -> 0
        top->clk = !top->clk;

        // Deassert reset after 4 clock ticks
        if (sim_time >= 8) {
            top->reset = 0;
        }

        // Evaluate the simulated hardware module
        top->eval();

        // Write current signal values to VCD waveform file
        tfp->dump(sim_time * 5); // 5ns per half-cycle = 10ns period (100 MHz sim clock)

        // Log events when clock rises
        if (top->clk == 1) {
            std::string event = "";
            if (top->reset) {
                event = "Reset active (LED held LOW)";
            } else if (top->led != last_led) {
                event = top->led ? "⚡ LED TOGGLED ON  [HIGH]" : "🌑 LED TOGGLED OFF [LOW]";
                last_led = top->led;
            }

            std::cout << std::setw(8) << (sim_time * 5)
                      << std::setw(8) << (int)top->clk
                      << std::setw(8) << (int)top->reset
                      << std::setw(8) << (int)top->led
                      << "   " << event << "\n";
        }

        sim_time++;
    }

    std::cout << std::string(55, '-') << "\n";
    std::cout << "\n✅ Simulation complete! Waveform saved to blinky.vcd\n";
    std::cout << "   Inspect waveforms visually by running: gtkwave blinky.vcd\n";

    tfp->close();
    top->final();

    return 0;
}
