#include "TestUtils.h"
#include "iDacInclude.h"

esp_err_t dac_output_enable(dac_channel_t channel) { return ESP_OK; }

esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value) {
    analogOutSignal = dac_value;
    return ESP_OK;
}

esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num) {
    *gpio_num = GPIO_NUM_NC;
    return ESP_OK;
}