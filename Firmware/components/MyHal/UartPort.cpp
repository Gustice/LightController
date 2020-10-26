
#include "iUartInclude.h"

#define BUF_SIZE 0x80

UartPort::UartPort(gpio_num_t txd, gpio_num_t rxd) : PortBase("HAL-Uart") {
    _uartH = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_config.source_clk = UART_SCLK_APB;

    uart_driver_install(_uartH, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(_uartH, &uart_config);
    uart_set_pin(_uartH, txd, rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    if (_uartH != 0) {
        InitOk = true;
    }
}

esp_err_t UartPort::TransmitSync(const uint8_t *txData, size_t len) {
    return uart_write_bytes(_uartH, (const char *)txData, len);
}

int UartPort::ReceiveSync(uint8_t *rxBuffer, size_t len) {
    return uart_read_bytes(_uartH, rxBuffer, len, 20 / portTICK_RATE_MS);
}