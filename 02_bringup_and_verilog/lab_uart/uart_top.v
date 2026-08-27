/**
 * lab_uart/uart_top.v
 *
 * Top-level UART Echo Server & Command Controller.
 * Connects uart_rx and uart_tx into a complete bidirectional serial interface:
 *   1. Receives incoming bytes from terminal (RX).
 *   2. Immediately echoes received bytes back to terminal (TX).
 *   3. Controls an LED: '1' turns LED ON, '0' turns LED OFF.
 */

module uart_top #(
    parameter CLKS_PER_BIT = 8
)(
    input  wire clk,            // System clock
    input  wire reset,          // Active-high reset
    input  wire uart_rx_pin,    // Physical serial input
    output wire uart_tx_pin,    // Physical serial output
    output reg  led_status      // Command-controlled LED status
);

    wire [7:0] rx_byte;
    wire       rx_byte_valid;
    wire       tx_busy;

    // Instantiate UART Receiver
    uart_rx #(
        .CLKS_PER_BIT(CLKS_PER_BIT)
    ) rx_inst (
        .clk(clk),
        .reset(reset),
        .rx_pin(uart_rx_pin),
        .rx_data(rx_byte),
        .rx_ready(rx_byte_valid)
    );

    // Instantiate UART Transmitter
    uart_tx #(
        .CLKS_PER_BIT(CLKS_PER_BIT)
    ) tx_inst (
        .clk(clk),
        .reset(reset),
        .tx_data(rx_byte),
        .tx_start(rx_byte_valid && !tx_busy), // Trigger transmit when byte arrives
        .tx_pin(uart_tx_pin),
        .tx_busy(tx_busy)
    );

    // Command Processor: Parse incoming characters for hardware control
    always @(posedge clk) begin
        if (reset) begin
            led_status <= 1'b0;
        end else if (rx_byte_valid) begin
            if (rx_byte == "1") begin
                led_status <= 1'b1; // Turn LED ON
            end else if (rx_byte == "0") begin
                led_status <= 1'b0; // Turn LED OFF
            end
        end
    end

endmodule
