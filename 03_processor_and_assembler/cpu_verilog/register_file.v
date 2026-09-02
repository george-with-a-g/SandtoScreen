/**
 * register_file.v — 16-Port 32-Bit ARM Register File (R0–R15)
 * Dual Read Ports + Single Write Port with R15 (PC) override.
 */
module register_file (
    input  wire        clk,
    input  wire        reset,

    // Read Port 1 (Rn)
    input  wire [3:0]  r1_addr,
    output wire [31:0] r1_data,

    // Read Port 2 (Rm / Rd for STR)
    input  wire [3:0]  r2_addr,
    output wire [31:0] r2_data,

    // Write Port (Rd)
    input  wire [3:0]  w_addr,
    input  wire [31:0] w_data,
    input  wire        w_en,

    // Program Counter Override (R15 returns current fetch PC)
    input  wire [31:0] pc_in
);

    // 16 32-bit registers (R0 to R15)
    reg [31:0] registers [0:15];
    integer i;

    // Asynchronous Read with R15 (PC) override
    assign r1_data = (r1_addr == 4'd15) ? pc_in : registers[r1_addr];
    assign r2_data = (r2_addr == 4'd15) ? pc_in : registers[r2_addr];

    // Synchronous Write Port
    always @(posedge clk) begin
        if (reset) begin
            for (i = 0; i < 16; i = i + 1) begin
                registers[i] <= 32'd0;
            end
        end else if (w_en && (w_addr != 4'd15)) begin
            // Writes to R0..R14 (R15/PC is updated by PC controller)
            registers[w_addr] <= w_data;
        end
    end

endmodule
