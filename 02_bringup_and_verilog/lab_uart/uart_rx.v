/**
 * lab_uart/uart_rx.v
 *
 * Universal Asynchronous Receiver-Transmitter (UART) Receiver.
 * Deserializes incoming asynchronous serial stream into an 8-bit parallel byte.
 *
 * Uses the Mid-Bit Sampling Technique:
 *   - Detects falling edge of Start Bit.
 *   - Samples at CLKS_PER_BIT / 2 (center of the eye diagram) for maximum noise immunity.
 *   - Samples each data bit every CLKS_PER_BIT cycles thereafter.
 */

module uart_rx #(
    parameter CLKS_PER_BIT = 8
)(
    input  wire       clk,          // System clock
    input  wire       reset,        // Active-high synchronous reset
    input  wire       rx_pin,       // Asynchronous serial input line (RX wire)
    output reg  [7:0] rx_data,      // 8-bit received byte
    output reg        rx_ready      // Pulses HIGH for 1 clock cycle when a byte is ready
);

    localparam STATE_IDLE  = 2'b00;
    localparam STATE_START = 2'b01;
    localparam STATE_DATA  = 2'b10;
    localparam STATE_STOP  = 2'b11;

    localparam CLK_WIDTH = $clog2(CLKS_PER_BIT);

    reg [1:0]           state;
    reg [CLK_WIDTH-1:0] clk_count;
    reg [2:0]           bit_index;
    reg [7:0]           rx_shift_reg;

    // 2-stage synchronizer to prevent metastability on asynchronous RX wire
    reg rx_sync_1, rx_sync_2;
    always @(posedge clk) begin
        if (reset) begin
            rx_sync_1 <= 1'b1;
            rx_sync_2 <= 1'b1;
        end else begin
            rx_sync_1 <= rx_pin;
            rx_sync_2 <= rx_sync_1;
        end
    end

    wire rx_stable = rx_sync_2;

    always @(posedge clk) begin
        if (reset) begin
            state        <= STATE_IDLE;
            rx_ready     <= 1'b0;
            clk_count    <= 0;
            bit_index    <= 0;
            rx_data      <= 8'h00;
            rx_shift_reg <= 8'h00;
        end else begin
            rx_ready <= 1'b0; // Default: pulse lasts only 1 clock cycle

            case (state)
                // ------------------------------------------------------------
                // 1. IDLE: Wait for Start Bit (Falling edge: 1 -> 0)
                // ------------------------------------------------------------
                STATE_IDLE: begin
                    clk_count <= 0;
                    bit_index <= 0;

                    if (rx_stable == 1'b0) begin
                        state <= STATE_START;
                    end
                end

                // ------------------------------------------------------------
                // 2. START BIT: Count to middle of bit (CLKS_PER_BIT / 2)
                // ------------------------------------------------------------
                STATE_START: begin
                    if (clk_count == (CLKS_PER_BIT - 1) / 2) begin
                        // Verify line is still LOW (filters false glitches)
                        if (rx_stable == 1'b0) begin
                            clk_count <= 0;
                            state     <= STATE_DATA;
                        end else begin
                            state     <= STATE_IDLE; // False alarm / noise
                        end
                    end else begin
                        clk_count <= clk_count + 1'b1;
                    end
                end

                // ------------------------------------------------------------
                // 3. DATA BITS: Sample in the center of each bit period
                // ------------------------------------------------------------
                STATE_DATA: begin
                    if (clk_count < CLKS_PER_BIT - 1) begin
                        clk_count <= clk_count + 1'b1;
                    end else begin
                        clk_count <= 0;
                        // Shift in received bit (LSB first)
                        rx_shift_reg[bit_index] <= rx_stable;

                        if (bit_index < 7) begin
                            bit_index <= bit_index + 1'b1;
                        end else begin
                            bit_index <= 0;
                            state     <= STATE_STOP;
                        end
                    end
                end

                // ------------------------------------------------------------
                // 4. STOP BIT: Sample stop bit and assert rx_ready
                // ------------------------------------------------------------
                STATE_STOP: begin
                    if (clk_count < CLKS_PER_BIT - 1) begin
                        clk_count <= clk_count + 1'b1;
                    end else begin
                        clk_count <= 0;
                        // Valid stop bit must be HIGH
                        if (rx_stable == 1'b1) begin
                            rx_data  <= rx_shift_reg;
                            rx_ready <= 1'b1; // Flag byte received!
                        end
                        state <= STATE_IDLE;
                    end
                end

                default: state <= STATE_IDLE;
            endcase
        end
    end

endmodule
