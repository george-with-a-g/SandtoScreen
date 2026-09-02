/**
 * sim_main.cpp — Verilator C++ Testbench for 32-Bit ARM7 CPU
 * Loads assembled hex binaries into RAM, clocks the processor, and traces execution.
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcpu_top.h"
#include "Vcpu_top___024root.h"

// Helper to load ASCII Hex file into CPU RAM
bool load_hex_file(Vcpu_top* top, const std::string& hex_filename) {
    std::ifstream file(hex_filename);
    if (!file.is_open()) {
        std::cerr << "❌ Error: Could not open hex file: " << hex_filename << "\n";
        return false;
    }

    std::string line;
    uint32_t addr = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        uint32_t word = std::stoul(line, nullptr, 16);
        top->rootp->cpu_top__DOT__u_mem__DOT__ram[addr] = word;
        addr++;
    }

    std::cout << "📥 Loaded " << addr << " instruction words into CPU RAM from '" << hex_filename << "'.\n";
    return true;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    std::string hex_file = "../assembler/examples/add.hex";
    if (argc > 1) {
        hex_file = argv[1];
    }

    Vcpu_top* top = new Vcpu_top;

    // Enable waveform tracing
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("cpu_trace.vcd");

    // Load assembled program into memory
    if (!load_hex_file(top, hex_file)) {
        return 1;
    }

    vluint64_t sim_time = 0;

    // Reset Sequence
    std::cout << "🔄 Resetting CPU...\n";
    top->reset = 1;
    top->clk = 0;
    top->uart_tx_busy = 0;
    top->uart_rx_valid = 0;

    for (int i = 0; i < 5; i++) {
        top->clk = !top->clk;
        top->eval();
        tfp->dump(sim_time++);
    }
    top->reset = 0;
    std::cout << "⚡ CPU Running...\n";

    // Run simulation for 100 clock cycles
    const int max_cycles = 100;
    for (int cycle = 0; cycle < max_cycles; cycle++) {
        // Clock Low
        top->clk = 0;
        top->eval();
        tfp->dump(sim_time++);

        // Clock High (Rising Edge)
        top->clk = 1;
        top->eval();
        tfp->dump(sim_time++);

        // Print UART transmission if triggered
        if (top->uart_tx_valid) {
            std::cout << "📡 [UART TX]: 0x" << std::hex << (int)top->uart_tx_byte 
                      << " ('" << (char)top->uart_tx_byte << "')\n" << std::dec;
        }
    }

    // Print Final CPU Register State
    std::cout << "\n=======================================================\n";
    std::cout << " 🏛️ FINAL CPU REGISTER STATE (After " << max_cycles << " Cycles)\n";
    std::cout << "=======================================================\n";
    std::cout << " PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_pc << "\n";
    std::cout << " R0: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_r0 << " (" << std::dec << top->dbg_r0 << ")\n";
    std::cout << " R1: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_r1 << " (" << std::dec << top->dbg_r1 << ")\n";
    std::cout << " R2: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_r2 << " (" << std::dec << top->dbg_r2 << ")\n";
    std::cout << " R3: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_r3 << " (" << std::dec << top->dbg_r3 << ")\n";
    std::cout << " R4: 0x" << std::hex << std::setw(8) << std::setfill('0') << top->dbg_r4 << " (" << std::dec << top->dbg_r4 << ")\n";
    std::cout << "=======================================================\n";
    std::cout << "📊 Waveform trace saved to 'cpu_trace.vcd'.\n\n";

    tfp->close();
    delete top;
    return 0;
}
