/**
 * lab_blinky/blinky.v
 *
 * A parameterized LED Blinker module demonstrating:
 *   1. Clock Division (Prescaling a high-frequency clock to human speed)
 *   2. Parameterization for simulation vs. physical hardware synthesis
 *   3. Synchronous reset and toggle flip-flops
 */

module blinky #(
    // Number of clock cycles per LED toggle.
    // On a real 50 MHz FPGA board: 50_000_000 / 2 = 25_000_000 cycles per toggle (1 Hz blink).
    // For fast simulation in Verilator: set to a small number like 10.
    parameter TOGGLE_LIMIT = 10
)(
    input  wire clk,        // Main system clock
    input  wire reset,      // Active-high synchronous reset
    output reg  led         // Output driving the physical LED
);

    // Calculate required counter bit-width at compile time
    // For TOGGLE_LIMIT = 10, $clog2(10) = 4 bits (counts 0 to 15)
    // For TOGGLE_LIMIT = 25_000_000, $clog2(25_000_000) = 25 bits
    localparam CNT_WIDTH = $clog2(TOGGLE_LIMIT);

    reg [CNT_WIDTH-1:0] counter;

    always @(posedge clk) begin
        if (reset) begin
            counter <= 0;
            led     <= 1'b0;
        end else begin
            if (counter >= TOGGLE_LIMIT - 1) begin
                counter <= 0;
                led     <= ~led; // Toggle LED state (0 -> 1 -> 0)
            end else begin
                counter <= counter + 1'b1;
            end
        end
    end

endmodule
