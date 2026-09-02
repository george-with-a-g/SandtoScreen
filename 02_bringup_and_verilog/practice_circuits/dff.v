/**
 * practice_circuits/dff.v
 * Task 3: 1-Bit Clocked D Flip-Flop with Synchronous Reset
 */
module dff (
    input  wire clk,
    input  wire reset,
    input  wire d,
    output reg  q
);

    always @(posedge clk) begin
        if (reset) begin
            q <= 1'b0;
        end else begin
            q <= d;
        end
    end

endmodule
