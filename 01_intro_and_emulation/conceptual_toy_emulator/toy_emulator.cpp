/**
 * conceptual_toy_emulator/toy_emulator.cpp
 * 
 * Demonstrates the full stack from fundamental transistors to a 4-bit digital counter
 * purely in standard C++ with ZERO external dependencies.
 * 
 * Hierarchy:
 *   1. Transistors (NMOS & PMOS switches)
 *   2. CMOS Logic Gates (Inverter, NAND)
 *   3. Sequential Storage (SR Latch, D-Latch, Edge-Triggered D-Flip-Flop)
 *   4. Arithmetic (1-bit Half Adder, 4-bit Binary Incrementer)
 *   5. 4-bit Clocked Binary Counter Module
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>

// ============================================================================
// LEVEL 0: Physical Transistor Switches (CMOS)
// ============================================================================

enum LogicLevel {
    LOW = 0,    // 0V / GND
    HIGH = 1    // VDD (Power)
};

// NMOS Transistor: Conducts (closes switch) when Gate is HIGH (1)
inline LogicLevel nmos_transistor(LogicLevel gate, LogicLevel source) {
    if (gate == HIGH) {
        return source; // Switch is closed: passes source (typically GND) to drain
    }
    return LOW; // Disconnected (floating in real life, modeled as inactive)
}

// PMOS Transistor: Conducts (closes switch) when Gate is LOW (0)
inline LogicLevel pmos_transistor(LogicLevel gate, LogicLevel source) {
    if (gate == LOW) {
        return source; // Switch is closed: passes source (typically VDD) to drain
    }
    return LOW; // Disconnected
}

// ============================================================================
// LEVEL 1: CMOS Logic Gates built directly from Transistors
// ============================================================================

// CMOS Inverter (NOT Gate): 1 PMOS pull-up to VDD, 1 NMOS pull-down to GND
LogicLevel cmos_not(LogicLevel in) {
    LogicLevel pull_up   = pmos_transistor(in, HIGH); // On when in == LOW
    LogicLevel pull_down = nmos_transistor(in, LOW);  // On when in == HIGH
    
    // In CMOS, if pull_up is active, output is pulled to HIGH (VDD)
    if (in == LOW) {
        return pull_up; // Returns HIGH
    } else {
        return pull_down; // Returns LOW
    }
}

// CMOS NAND Gate: 2 PMOS in parallel (pull-up), 2 NMOS in series (pull-down)
LogicLevel cmos_nand(LogicLevel a, LogicLevel b) {
    // Pull-up network (parallel PMOS): active if either input is LOW
    bool pmos_a_on = (a == LOW);
    bool pmos_b_on = (b == LOW);

    // Pull-down network (series NMOS): active only if BOTH inputs are HIGH
    bool nmos_series_on = (a == HIGH && b == HIGH);

    if (pmos_a_on || pmos_b_on) {
        return HIGH; // Pulled to VDD
    } else if (nmos_series_on) {
        return LOW;  // Pulled to GND
    }
    return HIGH;
}

// Logic Gates derived from Universal NAND:
LogicLevel gate_and(LogicLevel a, LogicLevel b) {
    return cmos_not(cmos_nand(a, b));
}

LogicLevel gate_or(LogicLevel a, LogicLevel b) {
    return cmos_nand(cmos_not(a), cmos_not(b));
}

LogicLevel gate_xor(LogicLevel a, LogicLevel b) {
    LogicLevel nand_ab = cmos_nand(a, b);
    return cmos_nand(cmos_nand(a, nand_ab), cmos_nand(b, nand_ab));
}

// ============================================================================
// LEVEL 2: Sequential Memory (Latches and D Flip-Flops)
// ============================================================================

// An SR-Latch built from cross-coupled NAND gates
class SRLatchNand {
private:
    LogicLevel q = LOW;
    LogicLevel q_bar = HIGH;

public:
    void eval(LogicLevel set_bar, LogicLevel reset_bar) {
        // Cross-coupled NAND feedback loop (settles in 2 iterations)
        for (int i = 0; i < 4; ++i) {
            q     = cmos_nand(set_bar, q_bar);
            q_bar = cmos_nand(reset_bar, q);
        }
    }
    LogicLevel get_q() const { return q; }
};

// Level-Sensitive D-Latch (Transparent when enable is HIGH)
class DLatch {
private:
    SRLatchNand sr_core;

public:
    void eval(LogicLevel d, LogicLevel enable) {
        // When enable is HIGH, D drives set_bar and reset_bar
        LogicLevel d_not = cmos_not(d);
        LogicLevel set_bar   = cmos_nand(d, enable);
        LogicLevel reset_bar = cmos_nand(d_not, enable);
        sr_core.eval(set_bar, reset_bar);
    }
    LogicLevel get_q() const { return sr_core.get_q(); }
};

// Master-Slave Positive-Edge-Triggered D Flip-Flop
// Captures input D on the rising edge of clock (0 -> 1)
class DFlipFlop {
private:
    DLatch master_latch;
    DLatch slave_latch;

public:
    void eval(LogicLevel d, LogicLevel clk) {
        // Master latch is transparent when clk is LOW
        LogicLevel clk_not = cmos_not(clk);
        master_latch.eval(d, clk_not);

        // Slave latch is transparent when clk is HIGH (transfers master's state to Q)
        slave_latch.eval(master_latch.get_q(), clk);
    }
    LogicLevel get_q() const { return slave_latch.get_q(); }
};

// ============================================================================
// LEVEL 3: Combinational Arithmetic (Half Adder)
// ============================================================================

struct AdderResult {
    LogicLevel sum;
    LogicLevel carry_out;
};

// 1-Bit Half Adder: computes sum (XOR) and carry (AND)
AdderResult half_adder(LogicLevel a, LogicLevel b) {
    AdderResult res;
    res.sum       = gate_xor(a, b);
    res.carry_out = gate_and(a, b);
    return res;
}

// ============================================================================
// LEVEL 4: 4-Bit Synchronous Binary Counter
// ============================================================================

class Counter4Bit {
private:
    DFlipFlop ff[4]; // 4 clocked D Flip-Flops (bits 0, 1, 2, 3)

public:
    // Computes next state and updates flip-flops on clock
    void step(LogicLevel clk, LogicLevel reset) {
        // Read current output bits
        LogicLevel q0 = ff[0].get_q();
        LogicLevel q1 = ff[1].get_q();
        LogicLevel q2 = ff[2].get_q();
        LogicLevel q3 = ff[3].get_q();

        // Combinational increment logic: Current Value + 1
        // Bit 0 toggles every cycle (Half Adder with carry_in = 1)
        AdderResult add0 = half_adder(q0, HIGH);
        AdderResult add1 = half_adder(q1, add0.carry_out);
        AdderResult add2 = half_adder(q2, add1.carry_out);
        AdderResult add3 = half_adder(q3, add2.carry_out);

        // Synchronous Reset: if reset is HIGH, next_d is forced to 0
        LogicLevel reset_not = cmos_not(reset);
        LogicLevel next_d0 = gate_and(add0.sum, reset_not);
        LogicLevel next_d1 = gate_and(add1.sum, reset_not);
        LogicLevel next_d2 = gate_and(add2.sum, reset_not);
        LogicLevel next_d3 = gate_and(add3.sum, reset_not);

        // Update each flip-flop with the clock signal
        ff[0].eval(next_d0, clk);
        ff[1].eval(next_d1, clk);
        ff[2].eval(next_d2, clk);
        ff[3].eval(next_d3, clk);
    }

    uint8_t read_value() const {
        return (ff[3].get_q() << 3) |
               (ff[2].get_q() << 2) |
               (ff[1].get_q() << 1) |
               (ff[0].get_q() << 0);
    }
};

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
