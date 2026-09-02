/**
 * practice_circuits/mux2.v
 * Task 2: 2-to-1 Multiplexer (Combinational Selection)
 */
module mux2 (
    input  wire a,      // Selected when sel == 0
    input  wire b,      // Selected when sel == 1
    input  wire sel,    // Selection wire
    output wire y       // Output
);

    // Continuous assignment using boolean logic or ternary operator
    assign y = sel ? b : a;

endmodule
