/**
 * @file DacPort.cpp
 * @author Gustice
 * @brief GPIO-Port classes implementation
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "iDacInclude.h"

DacPort::DacPort(dac_channel_t channel) : PortBase("HAL-Dac") {
    esp_err_t rErr;
    _channel = channel;

    rErr = dac_pad_get_io_num(channel, &_port);
    assert(rErr == ESP_OK);
    ESP_LOGI(cModTag, "DAC channel %d setup @GPIO %d ... ", _channel, _port);

    rErr = dac_output_enable(_channel);

    if (rErr == ESP_OK)
        InitOk = true;
    else
        ESP_LOGW(cModTag, "DAC channel issue %d: %s ", rErr, esp_err_to_name(rErr));
}

void DacPort::WritePort(uint8_t value) {
    esp_err_t rErr;
    rErr = dac_output_voltage(_channel, value);

    if (rErr != ESP_OK) {
        ESP_LOGD(cModTag, "DAC channel issue %d: %s ", rErr, esp_err_to_name(rErr));
    }
}
