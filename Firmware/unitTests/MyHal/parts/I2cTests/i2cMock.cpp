#include "TestUtils.h"
#include "iI2cInclude.h"

const size_t i2cBufSize = 8;
uint8_t virtualI2cBuffer[i2cBufSize];
uint8_t nextByte;

i2c_cmd_handle_t i2cHandle;

esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle,
                               TickType_t ticks_to_wait) {
    return ESP_OK;
}

i2c_cmd_handle_t i2c_cmd_link_create(void) { return i2cHandle; }

esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en) {
    return ESP_OK;
}

esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t *data, i2c_ack_type_t ack) {
    *data = nextByte; // This one is called on last byte without ACK-Flag
    return ESP_OK;
}

esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len,
                             size_t slv_tx_buf_len, int intr_alloc_flags) {
    return ESP_OK;
}

esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf) { return ESP_OK; }

void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle) {}

esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle) { return ESP_OK; }
esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle) { return ESP_OK; }

esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len,
                           bool ack_en) {
    memcpy(virtualI2cBuffer, data, data_len);
    return ESP_OK;
}

esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len,
                          i2c_ack_type_t ack) {
    memcpy(data, virtualI2cBuffer, data_len);
    nextByte = virtualI2cBuffer[data_len];
    return ESP_OK;
}