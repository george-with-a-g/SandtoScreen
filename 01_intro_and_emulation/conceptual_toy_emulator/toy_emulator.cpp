/**
 * conceptual_toy_emulator/toy_emulator.cpp
 * 
 * Demonstrates the full stack from fundamental transistors to a 4-bit digital counter
 * purely in standard C++ with ZERO external dependencies.
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>

#include "toy_emulator.hpp"

// ============================================================================
// Simulation Testbench & ASCII Output
// ============================================================================

int main() {
    std::cout << "==============================================================\n";
    std::cout << "   FROM THE TRANSISTOR TO A 4-BIT CLOCKED DIGITAL COUNTER     \n";
    std::cout << "   (Simulated from physical CMOS NMOS/PMOS transistor models) \n";
    std::cout << "==============================================================\n\n";

    Counter4Bit counter;

    std::cout << std::setw(6)  << "Step" 
              << std::setw(8)  << "Clock" 
              << std::setw(8)  << "Reset" 
              << std::setw(12) << "Binary Q" 
              << std::setw(10) << "Decimal" 
              << "   Waveform Track\n";
    std::cout << std::string(62, '-') << "\n";

    int cycle_count = 0;

    for (int step = 0; step < 36; ++step) {
        // Clock alternates LOW (0) and HIGH (1)
        LogicLevel clk = (step % 2 == 1) ? HIGH : LOW;
        
        // Assert reset for the first 2 steps, then release
        LogicLevel reset = (step < 4) ? HIGH : LOW;

        // Advance simulation
        counter.step(clk, reset);

        // Print status on every clock edge
        if (clk == HIGH) {
            uint8_t val = counter.read_value();
            std::string binary_str = "";
            for (int b = 3; b >= 0; --b) {
                binary_str += ((val >> b) & 1) ? '1' : '0';
            }

            std::string bar(val * 2, '#');

            std::cout << std::setw(6)  << cycle_count++
                      << std::setw(8)  << "HIGH"
                      << std::setw(8)  << (reset ? "ACTIVE" : "0")
                      << std::setw(12) << binary_str
                      << std::setw(10) << (int)val
                      << "   |" << bar << "\n";
        }
    }

    std::cout << std::string(62, '-') << "\n";
    std::cout << "\n✅ Simulation complete! You just watched physical transistor\n"
              << "   switches assemble into logic gates, memory latches, and\n"
              << "   a fully functioning 4-bit digital computer counter!\n";

    return 0;
}
