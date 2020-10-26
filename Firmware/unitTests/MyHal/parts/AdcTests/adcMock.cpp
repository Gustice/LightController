#include "TestUtils.h"
#include "iAdcInclude.h"

esp_err_t adc1_config_width(adc_bits_width_t width_bit) { return ESP_OK; }

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten) { return ESP_OK; }

int adc1_get_raw(adc1_channel_t channel) { return analogSignal; }

esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num) {
    *gpio_num = GPIO_NUM_NC;
    return ESP_OK;
}