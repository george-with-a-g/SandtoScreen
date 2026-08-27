/**
 * lab_uart/sim_main.cpp
 *
 * Verilator C++ testbench for UART Serial Echo Server & Command Controller.
 * Acts as a virtual PC terminal communicating over a simulated serial cable:
 *   1. Sends text bytes over simulated RX line.
 *   2. Deserializes and verifies the echoed bytes from TX line.
 *   3. Tests hardware LED command control ('1' and '0').
 *   4. Dumps digital waveform traces to uart.vcd.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vuart_top.h"

const int CLKS_PER_BIT = 8;

vluint64_t main_time = 0;
std::unique_ptr<Vuart_top> top;
std::unique_ptr<VerilatedVcdC> tfp;

// Helper to advance simulation by one clock cycle
void tick() {
    // Clock falling edge
    top->clk = 0;
    top->eval();
    tfp->dump(main_time * 5);
    main_time++;

    // Clock rising edge
    top->clk = 1;
    top->eval();
    tfp->dump(main_time * 5);
    main_time++;
}

// Software UART transmitter: sends 1 byte over the simulated RX wire
void send_uart_byte(uint8_t byte) {
    // 1. START BIT (LOW)
    top->uart_rx_pin = 0;
    for (int i = 0; i < CLKS_PER_BIT; ++i) tick();

    // 2. 8 DATA BITS (LSB first)
    for (int b = 0; b < 8; ++b) {
        top->uart_rx_pin = (byte >> b) & 1;
        for (int i = 0; i < CLKS_PER_BIT; ++i) tick();
    }

    // 3. STOP BIT (HIGH)
    top->uart_rx_pin = 1;
    for (int i = 0; i < CLKS_PER_BIT * 2; ++i) tick();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    top = std::make_unique<Vuart_top>();

    // Enable VCD tracing
    Verilated::traceEverOn(true);
    tfp = std::make_unique<VerilatedVcdC>();
    top->trace(tfp.get(), 99);
    tfp->open("uart.vcd");

    std::cout << "==============================================================\n";
    std::cout << "            RUNNING UART SERIAL ECHO TESTBENCH                \n";
    std::cout << "==============================================================\n\n";

    // Initial signals
    top->clk = 0;
    top->reset = 1;
    top->uart_rx_pin = 1; // Idle line is HIGH

    // Reset for 10 cycles
    for (int i = 0; i < 10; ++i) tick();
    top->reset = 0;
    for (int i = 0; i < 10; ++i) tick();

    std::string test_string = "HELLO VERILOG! 1 0";
    std::cout << "📡 Virtual Terminal transmitting string: \"" << test_string << "\"\n\n";

    std::string received_echo = "";

    // Track TX line deserialization in software
    int rx_state = 0; // 0=IDLE, 1=START, 2=DATA, 3=STOP
    int rx_clk_cnt = 0;
    int rx_bit_idx = 0;
    uint8_t rx_buf = 0;

    for (char c : test_string) {
        std::cout << "  -> Transmitted Byte: '" << c << "' (ASCII 0x" 
                  << std::hex << (int)(uint8_t)c << std::dec << ")\n";

        // Send character into simulated hardware
        send_uart_byte((uint8_t)c);

        // Run extra idle cycles while capturing echo
        for (int idle = 0; idle < CLKS_PER_BIT * 16; ++idle) {
            tick();

            // Software UART receiver monitoring hardware TX pin
            if (rx_state == 0) { // IDLE
                if (top->uart_tx_pin == 0) {
                    rx_state = 1;
                    rx_clk_cnt = 0;
                }
            } else if (rx_state == 1) { // START
                if (rx_clk_cnt == (CLKS_PER_BIT - 1) / 2) {
                    rx_state = 2;
                    rx_clk_cnt = 0;
                    rx_bit_idx = 0;
                    rx_buf = 0;
                } else {
                    rx_clk_cnt++;
                }
            } else if (rx_state == 2) { // DATA
                if (rx_clk_cnt < CLKS_PER_BIT - 1) {
                    rx_clk_cnt++;
                } else {
                    rx_clk_cnt = 0;
                    rx_buf |= (top->uart_tx_pin << rx_bit_idx);
                    if (rx_bit_idx < 7) {
                        rx_bit_idx++;
                    } else {
                        rx_state = 3;
                    }
                }
            } else if (rx_state == 3) { // STOP
                if (rx_clk_cnt < CLKS_PER_BIT - 1) {
                    rx_clk_cnt++;
                } else {
                    rx_state = 0;
                    received_echo += (char)rx_buf;
                    std::cout << "  <- Hardware Echoed:  '" << (char)rx_buf << "' | LED Status Pin: " 
                              << (int)top->led_status << "\n";
                }
            }
        }
    }

    std::cout << "\n--------------------------------------------------------------\n";
    std::cout << "Original Sent:   \"" << test_string << "\"\n";
    std::cout << "Hardware Echoed: \"" << received_echo << "\"\n";
    std::cout << "--------------------------------------------------------------\n";

    if (received_echo == test_string) {
        std::cout << "\n🎉 SUCCESS! All bytes perfectly echoed through UART hardware!\n";
    } else {
        std::cout << "\n❌ MISMATCH: Echoed string does not match transmitted string!\n";
    }

    std::cout << "   Waveform trace saved to uart.vcd (open with: gtkwave uart.vcd)\n";

    tfp->close();
    top->final();

    return (received_echo == test_string) ? 0 : 1;
}
