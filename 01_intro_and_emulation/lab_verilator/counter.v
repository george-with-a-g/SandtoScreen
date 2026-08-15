// 01_intro_and_emulation/lab_verilator/counter.v
// A simple 4-bit synchronous up-counter with enable and synchronous active-high reset.

`timescale 1ns / 1ps

module counter (
    input  wire       clk,     // Clock input
    input  wire       reset,   // Active-high synchronous reset
    input  wire       enable,  // Count enable signal
    output reg  [3:0] count    // 4-bit counter output (0 to 15)
);

    // Sequential logic: triggers on the rising edge of the clock
    always @(posedge clk) begin
        if (reset) begin
            count <= 4'b0000;
        end else if (enable) begin
            count <= count + 4'b0001;
        end
    end

endmodule
