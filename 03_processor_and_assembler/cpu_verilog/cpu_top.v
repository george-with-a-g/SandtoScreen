/**
 * cpu_top.v — Top-Level ARM7 SoC
 * Connects 3-Stage CPU Core to Memory Bus and MMIO Peripherals.
 */
module cpu_top (
    input  wire        clk,
    input  wire        reset,

    // UART Hardware Pins
    output wire [7:0]  uart_tx_byte,
    output wire        uart_tx_valid,
    input  wire        uart_tx_busy,
    input  wire [7:0]  uart_rx_byte,
    input  wire        uart_rx_valid,

    // Debug Ports
    output wire [31:0] dbg_pc,
    output wire [31:0] dbg_r0,
    output wire [31:0] dbg_r1,
    output wire [31:0] dbg_r2,
    output wire [31:0] dbg_r3,
    output wire [31:0] dbg_r4
);

    // Internal Memory Buses
    wire [31:0] inst_addr;
    wire [31:0] inst_rdata;
    wire [31:0] data_addr;
    wire [31:0] data_wdata;
    wire        data_we;
    wire        data_byte_en;
    wire [31:0] data_rdata;

    // CPU Core Instance
    cpu_core u_core (
        .clk         (clk),
        .reset       (reset),
        .inst_addr   (inst_addr),
        .inst_rdata  (inst_rdata),
        .mem_addr    (data_addr),
        .mem_wdata   (data_wdata),
        .mem_we      (data_we),
        .mem_byte_en (data_byte_en),
        .mem_rdata   (data_rdata),
        .dbg_pc      (dbg_pc),
        .dbg_r0      (dbg_r0),
        .dbg_r1      (dbg_r1),
        .dbg_r2      (dbg_r2),
        .dbg_r3      (dbg_r3),
        .dbg_r4      (dbg_r4)
    );

    // Memory & MMIO Bus Instance
    memory_bus u_mem (
        .clk           (clk),
        .reset         (reset),
        .inst_addr     (inst_addr),
        .inst_rdata    (inst_rdata),
        .data_addr     (data_addr),
        .data_wdata    (data_wdata),
        .data_we       (data_we),
        .data_byte_en  (data_byte_en),
        .data_rdata    (data_rdata),
        .uart_tx_byte  (uart_tx_byte),
        .uart_tx_valid (uart_tx_valid),
        .uart_tx_busy  (uart_tx_busy),
        .uart_rx_byte  (uart_rx_byte),
        .uart_rx_valid (uart_rx_valid)
    );

endmodule
