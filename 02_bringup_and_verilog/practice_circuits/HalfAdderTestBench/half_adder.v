/**
 * practice_circuits/half_adder.v
 * Task 4: 1-Bit Half Adder (Sum & Carry Out)
 */
module half_adder (
    input  wire a,
    input  wire b,
    output wire sum,
    output wire carry
);

    assign sum   = a ^ b; // XOR gate
    assign carry = a & b; // AND gate

endmodule
