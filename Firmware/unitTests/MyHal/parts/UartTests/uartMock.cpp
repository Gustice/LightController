#include "TestUtils.h"
#include "iUartInclude.h"

const size_t uartBufLen = 8;
uint8_t virtualUartBuffer[uartBufLen];

esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size,
                              int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags) {
    return ESP_OK;
}

esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config) {
    return ESP_OK;
}

esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num,
                       int cts_io_num) {
    return ESP_OK;
}

int uart_write_bytes(uart_port_t uart_num, const char *src, size_t size) {
    memcpy(virtualUartBuffer, src, size);
    return (int)size;
}

int uart_read_bytes(uart_port_t uart_num, uint8_t *buf, uint32_t length, TickType_t ticks_to_wait) {
    memcpy(buf, virtualUartBuffer, length);
    return 0;
}
