/**
 * cpu_core.v — 32-Bit ARM7 (ARMv4T) 3-Stage Pipelined Processor Core
 * Fetch -> Decode -> Execute
 */
module cpu_core (
    input  wire        clk,
    input  wire        reset,

    // Instruction Memory Bus (Fetch Stage)
    output wire [31:0] inst_addr,
    input  wire [31:0] inst_rdata,

    // Data Memory Bus (Execute Stage - RAM & MMIO)
    output wire [31:0] mem_addr,
    output wire [31:0] mem_wdata,
    output wire        mem_we,
    output wire        mem_byte_en,
    input  wire [31:0] mem_rdata,

    // Debug / Tracing Ports
    output wire [31:0] dbg_pc,
    output wire [31:0] dbg_r0,
    output wire [31:0] dbg_r1,
    output wire [31:0] dbg_r2,
    output wire [31:0] dbg_r3,
    output wire [31:0] dbg_r4
);

    // ========================================================================
    // 1. PROGRAM COUNTER & FETCH STAGE
    // ========================================================================
    reg [31:0] pc;
    wire [31:0] pc_next;
    wire [31:0] pc_plus4 = pc + 32'd4;

    assign inst_addr = pc;
    assign dbg_pc    = pc;

    // Pipeline Registers: Fetch -> Decode
    reg [31:0] fd_inst;
    reg [31:0] fd_pc;
    reg        fd_valid;

    always @(posedge clk) begin
        if (reset || branch_taken) begin
            fd_inst  <= 32'hE1A00000; // NOP (MOV R0, R0)
            fd_pc    <= 32'd0;
            fd_valid <= 1'b0;
        end else begin
            fd_inst  <= inst_rdata;
            fd_pc    <= pc;
            fd_valid <= 1'b1;
        end
    end

    // ========================================================================
    // 2. DECODE STAGE
    // ========================================================================
    wire [3:0]  dec_cond;
    wire        dec_is_dp;
    wire        dec_is_mem;
    wire        dec_is_branch;
    wire        dec_is_imm;
    wire [3:0]  dec_opcode;
    wire        dec_s_bit;
    wire [3:0]  dec_rn;
    wire [3:0]  dec_rd;
    wire [3:0]  dec_rm;
    wire [31:0] dec_imm32;
    wire        dec_mem_load;
    wire        dec_mem_byte;
    wire [11:0] dec_mem_offset;
    wire        dec_branch_link;
    wire [23:0] dec_branch_offset;

    decoder u_decoder (
        .inst             (fd_inst),
        .cond             (dec_cond),
        .is_data_proc     (dec_is_dp),
        .is_load_store    (dec_is_mem),
        .is_branch        (dec_is_branch),
        .is_imm           (dec_is_imm),
        .opcode           (dec_opcode),
        .s_bit            (dec_s_bit),
        .rn_addr          (dec_rn),
        .rd_addr          (dec_rd),
        .rm_addr          (dec_rm),
        .imm32            (dec_imm32),
        .mem_is_load      (dec_mem_load),
        .mem_is_byte      (dec_mem_byte),
        .mem_offset       (dec_mem_offset),
        .branch_link      (dec_branch_link),
        .branch_offset    (dec_branch_offset)
    );

    // Register File Instance
    wire [31:0] rf_r1_data;
    wire [31:0] rf_r2_data;
    wire [31:0] rf_wdata;
    wire        rf_we;
    wire [3:0]  rf_waddr;

    register_file u_rf (
        .clk     (clk),
        .reset   (reset),
        .r1_addr (dec_rn),
        .r1_data (rf_r1_data),
        .r2_addr (dec_is_mem ? dec_rd : dec_rm),
        .r2_data (rf_r2_data),
        .w_addr  (rf_waddr),
        .w_data  (rf_wdata),
        .w_en    (rf_we),
        .pc_in   (pc + 32'd8) // ARM PC+8 Rule
    );

    // Data Forwarding (Bypassing) from Execute stage to Decode stage
    wire forward_r1 = rf_we && (rf_waddr != 4'd15) && (rf_waddr == dec_rn);
    wire forward_r2 = rf_we && (rf_waddr != 4'd15) && (rf_waddr == (dec_is_mem ? dec_rd : dec_rm));

    wire [31:0] actual_r1_data = forward_r1 ? rf_wdata : rf_r1_data;
    wire [31:0] actual_r2_data = forward_r2 ? rf_wdata : rf_r2_data;

    // Pipeline Registers: Decode -> Execute
    reg [31:0] de_pc;
    reg [31:0] de_inst;
    reg [3:0]  de_cond;
    reg        de_is_dp;
    reg        de_is_mem;
    reg        de_is_branch;
    reg        de_is_imm;
    reg [3:0]  de_opcode;
    reg        de_s_bit;
    reg [3:0]  de_rn;
    reg [3:0]  de_rd;
    reg [31:0] de_r1_data;
    reg [31:0] de_r2_data;
    reg [31:0] de_imm32;
    reg        de_mem_load;
    reg        de_mem_byte;
    reg [11:0] de_mem_offset;
    reg        de_branch_link;
    reg [23:0] de_branch_offset;
    reg        de_valid;

    always @(posedge clk) begin
        if (reset || branch_taken) begin
            de_valid        <= 1'b0;
            de_inst         <= 32'hE1A00000;
            de_is_dp        <= 1'b0;
            de_is_mem       <= 1'b0;
            de_is_branch    <= 1'b0;
            de_cond         <= 4'hE;
            de_rd           <= 4'd0;
            de_s_bit        <= 1'b0;
        end else begin
            de_valid        <= fd_valid;
            de_pc           <= fd_pc;
            de_inst         <= fd_inst;
            de_cond         <= dec_cond;
            de_is_dp        <= dec_is_dp;
            de_is_mem       <= dec_is_mem;
            de_is_branch    <= dec_is_branch;
            de_is_imm       <= dec_is_imm;
            de_opcode       <= dec_opcode;
            de_s_bit        <= dec_s_bit;
            de_rn           <= dec_rn;
            de_rd           <= dec_rd;
            de_r1_data      <= actual_r1_data;
            de_r2_data      <= actual_r2_data;
            de_imm32        <= dec_imm32;
            de_mem_load     <= dec_mem_load;
            de_mem_byte     <= dec_mem_byte;
            de_mem_offset   <= dec_mem_offset;
            de_branch_link  <= dec_branch_link;
            de_branch_offset<= dec_branch_offset;
        end
    end

    // ========================================================================
    // 3. EXECUTE STAGE
    // ========================================================================

    // CPSR Condition Code Flags
    reg flag_n, flag_z, flag_c, flag_v;

    // Evaluate Condition Code
    reg cond_pass;
    always @(*) begin
        case (de_cond)
            4'h0: cond_pass = flag_z;                               // EQ
            4'h1: cond_pass = !flag_z;                              // NE
            4'h2: cond_pass = flag_c;                               // CS / HS
            4'h3: cond_pass = !flag_c;                              // CC / LO
            4'h4: cond_pass = flag_n;                               // MI
            4'h5: cond_pass = !flag_n;                              // PL
            4'h6: cond_pass = flag_v;                               // VS
            4'h7: cond_pass = !flag_v;                              // VC
            4'h8: cond_pass = flag_c && !flag_z;                    // HI
            4'h9: cond_pass = !flag_c || flag_z;                    // LS
            4'hA: cond_pass = (flag_n == flag_v);                   // GE
            4'hB: cond_pass = (flag_n != flag_v);                   // LT
            4'hC: cond_pass = !flag_z && (flag_n == flag_v);        // GT
            4'hD: cond_pass = flag_z || (flag_n != flag_v);         // LE
            4'hE: cond_pass = 1'b1;                                 // AL (Always)
            default: cond_pass = 1'b1;
        endcase
    end

    // ALU Operands & Instance
    wire [31:0] alu_op_b = de_is_imm ? de_imm32 : de_r2_data;
    wire [31:0] alu_result;
    wire alu_n, alu_z, alu_c, alu_v;

    alu u_alu (
        .a        (de_r1_data),
        .b        (alu_op_b),
        .opcode   (de_opcode),
        .carry_in (flag_c),
        .result   (alu_result),
        .flag_n   (alu_n),
        .flag_z   (alu_z),
        .flag_c   (alu_c),
        .flag_v   (alu_v)
    );

    // Update CPSR Flags
    always @(posedge clk) begin
        if (reset) begin
            flag_n <= 1'b0;
            flag_z <= 1'b0;
            flag_c <= 1'b0;
            flag_v <= 1'b0;
        end else if (de_valid && cond_pass && de_is_dp && de_s_bit) begin
            flag_n <= alu_n;
            flag_z <= alu_z;
            flag_c <= alu_c;
            flag_v <= alu_v;
        end
    end

    // Branch Target Calculation
    wire [31:0] branch_target = de_pc + 32'd8 + {{6{de_branch_offset[23]}}, de_branch_offset, 2'b00};
    wire branch_taken = de_valid && cond_pass && de_is_branch;

    // Memory Bus Outputs
    assign mem_addr    = de_r1_data + {20'd0, de_mem_offset};
    assign mem_wdata   = de_r2_data;
    assign mem_we      = de_valid && cond_pass && de_is_mem && !de_mem_load;
    assign mem_byte_en = de_mem_byte;

    // Register Writeback
    wire is_compare = (de_opcode == 4'h8 || de_opcode == 4'h9 || de_opcode == 4'hA || de_opcode == 4'hB);
    assign rf_we    = de_valid && cond_pass && ((de_is_dp && !is_compare) || (de_is_mem && de_mem_load) || (de_is_branch && de_branch_link));
    assign rf_waddr = (de_is_branch && de_branch_link) ? 4'd14 : de_rd; // LR for BL
    assign rf_wdata = (de_is_branch && de_branch_link) ? (de_pc + 32'd4) :
                      (de_is_mem && de_mem_load)       ? mem_rdata : alu_result;

    // Program Counter Next Logic
    always @(posedge clk) begin
        if (reset) begin
            pc <= 32'd0;
        end else if (branch_taken) begin
            pc <= branch_target;
        end else begin
            pc <= pc_plus4;
        end
    end

    // Debug register readouts (R0..R4)
    assign dbg_r0 = u_rf.registers[0];
    assign dbg_r1 = u_rf.registers[1];
    assign dbg_r2 = u_rf.registers[2];
    assign dbg_r3 = u_rf.registers[3];
    assign dbg_r4 = u_rf.registers[4];

endmodule
