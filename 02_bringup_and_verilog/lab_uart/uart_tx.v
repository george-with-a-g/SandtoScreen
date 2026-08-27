/**
 * lab_uart/uart_tx.v
 *
 * Universal Asynchronous Receiver-Transmitter (UART) Transmitter.
 * Serializes 8-bit parallel byte data into 8-N-1 serial framing:
 *   - 1 Start Bit (LOW / 0)
 *   - 8 Data Bits (LSB first)
 *   - 1 Stop Bit (HIGH / 1)
 */

module uart_tx #(
    // Clock cycles per serial bit.
    // For 50 MHz clock @ 115200 Baud: 50_000_000 / 115200 = ~434 cycles.
    // For fast simulation in Verilator: set to small number like 8.
    parameter CLKS_PER_BIT = 8
)(
    input  wire       clk,          // System clock
    input  wire       reset,        // Active-high synchronous reset
    input  wire [7:0] tx_data,      // 8-bit byte to transmit
    input  wire       tx_start,     // Pulse HIGH for 1 cycle to begin transmission
    output reg        tx_pin,       // Serial physical output line (TX wire)
    output reg        tx_busy       // HIGH while transmitting, LOW when ready for new byte
);

    localparam STATE_IDLE  = 2'b00;
    localparam STATE_START = 2'b01;
    localparam STATE_DATA  = 2'b10;
    localparam STATE_STOP  = 2'b11;

    localparam CLK_WIDTH = $clog2(CLKS_PER_BIT);

    reg [1:0]           state;
    reg [CLK_WIDTH-1:0] clk_count;
    reg [2:0]           bit_index;
    reg [7:0]           data_buffer;

    always @(posedge clk) begin
        if (reset) begin
            state       <= STATE_IDLE;
            tx_pin      <= 1'b1; // Idle line is HIGH in UART standard
            tx_busy     <= 1'b0;
            clk_count   <= 0;
            bit_index   <= 0;
            data_buffer <= 8'h00;
        end else begin
            case (state)
                // ------------------------------------------------------------
                // 1. IDLE: Wait for start command
                // ------------------------------------------------------------
                STATE_IDLE: begin
                    tx_pin    <= 1'b1;
                    tx_busy   <= 1'b0;
                    clk_count <= 0;
                    bit_index <= 0;

                    if (tx_start) begin
                        data_buffer <= tx_data;
                        tx_busy     <= 1'b1;
                        state       <= STATE_START;
                    end
                end

                // ------------------------------------------------------------
                // 2. START BIT: Drive line LOW for 1 baud period
                // ------------------------------------------------------------
                STATE_START: begin
                    tx_pin <= 1'b0;

                    if (clk_count < CLKS_PER_BIT - 1) begin
                        clk_count <= clk_count + 1'b1;
                    end else begin
                        clk_count <= 0;
                        state     <= STATE_DATA;
                    end
                end

                // ------------------------------------------------------------
                // 3. DATA BITS: Transmit 8 data bits (LSB first)
                // ------------------------------------------------------------
                STATE_DATA: begin
                    tx_pin <= data_buffer[bit_index];

                    if (clk_count < CLKS_PER_BIT - 1) begin
                        clk_count <= clk_count + 1'b1;
                    end else begin
                        clk_count <= 0;
                        if (bit_index < 7) begin
                            bit_index <= bit_index + 1'b1;
                        end else begin
                            bit_index <= 0;
                            state     <= STATE_STOP;
                        end
                    end
                end

                // ------------------------------------------------------------
                // 4. STOP BIT: Drive line HIGH for 1 baud period
                // ------------------------------------------------------------
                STATE_STOP: begin
                    tx_pin <= 1'b1;

                    if (clk_count < CLKS_PER_BIT - 1) begin
                        clk_count <= clk_count + 1'b1;
                    end else begin
                        clk_count <= 0;
                        state     <= STATE_IDLE; // Done transmitting!
                    end
                end

                default: state <= STATE_IDLE;
            endcase
        end
    end

endmodule
