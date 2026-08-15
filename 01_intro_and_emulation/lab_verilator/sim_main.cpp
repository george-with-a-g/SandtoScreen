// 01_intro_and_emulation/lab_verilator/sim_main.cpp
// C++ Testbench for Verilator simulation of counter.v

#include <iostream>
#include <iomanip>
#include <memory>
#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    // Pass command line arguments to Verilator
    Verilated::commandArgs(argc, argv);

    // 1. Instantiate the Verilog hardware module
    auto top = std::make_unique<Vcounter>();

    // 2. Setup VCD waveform tracing
    Verilated::traceEverOn(true);
    auto tfp = std::make_unique<VerilatedVcdC>();
    top->trace(tfp.get(), 99); // Trace 99 levels of hierarchy
    tfp->open("counter.vcd");

    std::cout << "====================================================\n";
    std::cout << "       VERILATOR 4-BIT COUNTER SIMULATION           \n";
    std::cout << "====================================================\n";
    std::cout << "Time(ns)  CLK  RESET  ENABLE  COUNT(Hex)  COUNT(Dec)\n";
    std::cout << "----------------------------------------------------\n";

    vluint64_t sim_time = 0; // Simulation time in nanoseconds
    const vluint64_t max_time = 80;

    while (sim_time < max_time) {
        // Toggle clock every 5 ns (100 MHz clock -> period = 10 ns)
        top->clk = (sim_time % 10 < 5) ? 0 : 1;

        // Drive inputs:
        // Hold reset HIGH for the first 15 ns, then release
        top->reset = (sim_time < 15) ? 1 : 0;

        // Keep enable HIGH throughout
        top->enable = 1;

        // Evaluate the circuit at the current timestamp
        top->eval();

        // Dump signals to VCD file for GTKWave
        tfp->dump(sim_time);

        // Print outputs on rising clock edges
        if (top->clk == 1 && (sim_time % 10 == 5)) {
            std::cout << std::setw(8) << sim_time << "    "
                      << (int)top->clk << "     "
                      << (int)top->reset << "      "
                      << (int)top->enable << "        0x"
                      << std::hex << (int)top->count << "        "
                      << std::dec << (int)top->count << "\n";
        }

        sim_time++;
    }

    std::cout << "----------------------------------------------------\n";
    std::cout << "Simulation finished successfully!\n";
    std::cout << "Generated VCD trace: counter.vcd\n";
    std::cout << "View waveform with: gtkwave counter.vcd\n";

    // Clean up
    tfp->close();
    return 0;
}
