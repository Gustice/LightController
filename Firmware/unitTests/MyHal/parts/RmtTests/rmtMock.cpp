#include "TestUtils.h"
#include "iRmtInclude.h"

uint8_t virtualRmtTxBuffer[256];
size_t RmtSize;
uint32_t RmtTimeout;

esp_err_t rmt_config(const rmt_config_t *rmt_param) { return ESP_OK; }

esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags) {
    return ESP_OK;
}

esp_err_t rmt_write_sample(rmt_channel_t channel, const uint8_t *src, size_t src_size,
                           bool wait_tx_done) {
    RmtSize = src_size;
    for (size_t i = 0; i < src_size; i++) {
        virtualRmtTxBuffer[i] = src[i];
    }
    return ESP_OK;
}

esp_err_t rmt_wait_tx_done(rmt_channel_t channel, TickType_t wait_time) {
    RmtTimeout = wait_time;
    return ESP_OK;
}

esp_err_t rmt_get_counter_clock(rmt_channel_t channel, uint32_t *clock_hz) {
    *clock_hz = 8000000;
    return ESP_OK;
}

esp_err_t rmt_translator_init(rmt_channel_t channel, sample_to_rmt_t fn) { return ESP_OK; }


