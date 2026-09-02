/**
 * practice_circuits/inverter.v
 * Task 1: Hello World of Verilog (1-Bit Inverter / NOT Gate)
 */
module inverter (
    input  wire in,
    output wire out
);

    // Invert the input signal
    assign out = ~in;

endmodule
