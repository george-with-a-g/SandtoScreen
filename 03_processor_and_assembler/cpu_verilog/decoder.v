/**
 * decoder.v — ARM32 Instruction Decoder
 * Decodes a 32-bit machine code instruction into control and data signals.
 */
module decoder (
    input  wire [31:0] inst,

    // Condition Field
    output wire [3:0]  cond,

    // Instruction Types
    output wire        is_data_proc,
    output wire        is_load_store,
    output wire        is_branch,

    // Data Processing Fields
    output wire        is_imm,
    output wire [3:0]  opcode,
    output wire        s_bit,
    output wire [3:0]  rn_addr,
    output wire [3:0]  rd_addr,
    output wire [3:0]  rm_addr,
    output wire [31:0] imm32,

    // Memory Fields
    output wire        mem_is_load,
    output wire        mem_is_byte,
    output wire [11:0] mem_offset,

    // Branch Fields
    output wire        branch_link,
    output wire [23:0] branch_offset
);

    assign cond = inst[31:28];

    // Primary opcode classification (bits 27:26)
    assign is_data_proc  = (inst[27:26] == 2'b00);
    assign is_load_store = (inst[27:26] == 2'b01);
    assign is_branch     = (inst[27:25] == 3'b101);

    // Register addresses
    assign rn_addr = inst[19:16];
    assign rd_addr = inst[15:12];
    assign rm_addr = inst[3:0];

    // Data processing specifics
    assign is_imm  = inst[25];
    assign opcode  = inst[24:21];
    assign s_bit   = inst[20];

    // 8-bit immediate with 4-bit rotate (right rotate by 2 * rot)
    wire [7:0] imm8 = inst[7:0];
    wire [3:0] rot  = inst[11:8];
    wire [4:0] rot_amount = {rot, 1'b0}; // rot * 2
    assign imm32 = (rot == 4'd0) ? {24'd0, imm8} :
                   ({24'd0, imm8} >> rot_amount) | ({24'd0, imm8} << (32 - rot_amount));

    // Memory access specifics
    assign mem_is_load = inst[20];
    assign mem_is_byte = inst[22];
    assign mem_offset  = inst[11:0];

    // Branch specifics
    assign branch_link   = inst[24];
    assign branch_offset = inst[23:0];

endmodule
