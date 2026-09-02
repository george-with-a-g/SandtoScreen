/**
 * memory_bus.v — Unified Memory & Memory-Mapped I/O (MMIO) Controller
 * Address Map:
 *   0x0000_0000 - 0x0000_FFFF: 64 KB Code / Data RAM
 *   0x1000_0000: UART Data Register (Write: TX byte, Read: RX byte)
 *   0x1000_0004: UART Status Register (Bit 0: TX Ready, Bit 1: RX Data Valid)
 */
module memory_bus (
    input  wire        clk,
    input  wire        reset,

    // CPU Instruction Fetch Port
    input  wire [31:0] inst_addr,
    output wire [31:0] inst_rdata,

    // CPU Data Memory Port
    input  wire [31:0] data_addr,
    input  wire [31:0] data_wdata,
    input  wire        data_we,
    input  wire        data_byte_en,
    output reg  [31:0] data_rdata,

    // UART Hardware Ports
    output reg  [7:0]  uart_tx_byte,
    output reg         uart_tx_valid,
    input  wire        uart_tx_busy,
    input  wire [7:0]  uart_rx_byte,
    input  wire        uart_rx_valid
);

    // 64 KB RAM (16,384 32-bit words)
    reg [31:0] ram [0:16383];

    // Instruction Fetch (Dual-ported read from RAM)
    wire [13:0] inst_word_addr = inst_addr[15:2];
    assign inst_rdata = ram[inst_word_addr];

    // Data RAM Address
    wire [13:0] data_word_addr = data_addr[15:2];
    wire is_ram_access  = (data_addr < 32'h00010000);
    wire is_uart_data   = (data_addr == 32'h10000000);
    wire is_uart_status = (data_addr == 32'h10000004);

    // Read Data Multiplexer
    always @(*) begin
        if (is_ram_access) begin
            data_rdata = ram[data_word_addr];
        end else if (is_uart_data) begin
            data_rdata = {24'd0, uart_rx_byte};
        end else if (is_uart_status) begin
            data_rdata = {30'd0, uart_rx_valid, !uart_tx_busy};
        end else begin
            data_rdata = 32'd0;
        end
    end

    // RAM and MMIO Write Logic
    integer i;
    always @(posedge clk) begin
        if (reset) begin
            uart_tx_valid <= 1'b0;
            uart_tx_byte  <= 8'd0;
        end else begin
            uart_tx_valid <= 1'b0;

            if (data_we) begin
                if (is_ram_access) begin
                    if (data_byte_en) begin
                        // Byte write (based on bottom 2 address bits)
                        case (data_addr[1:0])
                            2'b00: ram[data_word_addr][7:0]   <= data_wdata[7:0];
                            2'b01: ram[data_word_addr][15:8]  <= data_wdata[7:0];
                            2'b10: ram[data_word_addr][23:16] <= data_wdata[7:0];
                            2'b11: ram[data_word_addr][31:24] <= data_wdata[7:0];
                        endcase
                    end else begin
                        // 32-bit Word write
                        ram[data_word_addr] <= data_wdata;
                    end
                end else if (is_uart_data) begin
                    // MMIO Write to UART Transmit Register
                    uart_tx_byte  <= data_wdata[7:0];
                    uart_tx_valid <= 1'b1;
                end
            end
        end
    end

endmodule
