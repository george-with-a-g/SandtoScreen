/**
 * alu.v — 32-Bit Arithmetic Logic Unit (ALU)
 * Implements all standard ARM7 Data Processing operations and CPSR condition flags.
 */
module alu (
    input  wire [31:0] a,          // Operand A (Rn)
    input  wire [31:0] b,          // Operand B (Operand2 / Rm / Immediate)
    input  wire [3:0]  opcode,     // ARM Data Processing Opcode (0..15)
    input  wire        carry_in,   // Carry flag from CPSR
    output reg  [31:0] result,     // 32-bit computed result
    output wire        flag_n,     // Negative flag
    output wire        flag_z,     // Zero flag
    output reg         flag_c,     // Carry out flag
    output reg         flag_v      // Overflow flag
);

    reg [32:0] sum_extended;

    always @(*) begin
        // Defaults
        sum_extended = 33'd0;
        flag_c       = carry_in;
        flag_v       = 1'b0;

        case (opcode)
            4'b0000: result = a & b;                         // AND
            4'b0001: result = a ^ b;                         // EOR (XOR)
            4'b0010: begin                                   // SUB: a - b
                sum_extended = {1'b0, a} - {1'b0, b};
                result       = sum_extended[31:0];
                flag_c       = (a >= b);                     // ARM SUB: Carry is NOT-borrow
                flag_v       = (a[31] != b[31]) && (result[31] != a[31]);
            end
            4'b0011: begin                                   // RSB: b - a (Reverse Subtract)
                sum_extended = {1'b0, b} - {1'b0, a};
                result       = sum_extended[31:0];
                flag_c       = (b >= a);
                flag_v       = (b[31] != a[31]) && (result[31] != b[31]);
            end
            4'b0100: begin                                   // ADD: a + b
                sum_extended = {1'b0, a} + {1'b0, b};
                result       = sum_extended[31:0];
                flag_c       = sum_extended[32];             // Unsigned carry out
                flag_v       = (a[31] == b[31]) && (result[31] != a[31]); // Signed overflow
            end
            4'b0101: begin                                   // ADC: a + b + carry_in
                sum_extended = {1'b0, a} + {1'b0, b} + {32'd0, carry_in};
                result       = sum_extended[31:0];
                flag_c       = sum_extended[32];
                flag_v       = (a[31] == b[31]) && (result[31] != a[31]);
            end
            4'b0110: begin                                   // SBC: a - b + carry_in - 1
                sum_extended = {1'b0, a} - {1'b0, b} - {32'd0, !carry_in};
                result       = sum_extended[31:0];
                flag_c       = (a >= (b + {31'd0, !carry_in}));
                flag_v       = (a[31] != b[31]) && (result[31] != a[31]);
            end
            4'b0111: begin                                   // RSC: b - a + carry_in - 1
                sum_extended = {1'b0, b} - {1'b0, a} - {32'd0, !carry_in};
                result       = sum_extended[31:0];
                flag_c       = (b >= (a + {31'd0, !carry_in}));
                flag_v       = (b[31] != a[31]) && (result[31] != b[31]);
            end
            4'b1000: result = a & b;                         // TST (Test bits)
            4'b1001: result = a ^ b;                         // TEQ (Test equivalence)
            4'b1010: begin                                   // CMP: a - b (Compare)
                sum_extended = {1'b0, a} - {1'b0, b};
                result       = sum_extended[31:0];
                flag_c       = (a >= b);
                flag_v       = (a[31] != b[31]) && (result[31] != a[31]);
            end
            4'b1011: begin                                   // CMN: a + b (Compare Negated)
                sum_extended = {1'b0, a} + {1'b0, b};
                result       = sum_extended[31:0];
                flag_c       = sum_extended[32];
                flag_v       = (a[31] == b[31]) && (result[31] != a[31]);
            end
            4'b1100: result = a | b;                         // ORR
            4'b1101: result = b;                             // MOV
            4'b1110: result = a & ~b;                        // BIC (Bit Clear)
            4'b1111: result = ~b;                            // MVN (Move Not)
            default: result = 32'd0;
        endcase
    end

    // Flag outputs
    assign flag_n = result[31];             // Negative if MSB is 1
    assign flag_z = (result == 32'd0);      // Zero if result is exactly 0

endmodule
