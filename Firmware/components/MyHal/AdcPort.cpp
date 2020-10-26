
/**
 * @file AdcPort.cpp
 * @author Gustice
 * @brief ADC-Port classes implementation
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#include "iAdcInclude.h"

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
// ADC_ATTEN_11db => FullScale = 3.3 V (supply on VDD_2)
static const adc_atten_t attenuation = ADC_ATTEN_DB_11;

AdcPort::AdcPort(adc1_channel_t channel) : PortBase("HAL-Adc") {
    _channel = (adc_channel_t)channel;
    _unit = ADC_UNIT_1;
    assert(adc1_pad_get_io_num(channel, &_port) == ESP_OK);

    ESP_LOGI(cModTag, "ADC_WIDTH_BIT_12 channel %d setup @GPIO %d ... ", channel, _port);

    assert(adc1_config_width(width) == ESP_OK);

    esp_err_t rErr = adc1_config_channel_atten((adc1_channel_t)_channel, attenuation);
    if (rErr == ESP_OK)
        InitOk = true;
    else
        ESP_LOGW(cModTag, "ADC1 channel setup issue %d: %s ", rErr, esp_err_to_name(rErr));
}

uint16_t AdcPort::ReadPort(void) {
    esp_err_t rErr = ESP_ERR_NOT_FOUND;
    int read_raw = -1;

    if (_unit == adc_unit_t::ADC_UNIT_1) {
        read_raw = adc1_get_raw((adc1_channel_t)_channel);
        // uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    } else if (_unit == adc_unit_t::ADC_UNIT_2) {
        ESP_LOGE(cModTag, "ADC2 channel not implemented yet");
        // esp_err_t rErr;
        // int read_raw;

        // rErr = adc2_get_raw( (adc2_channel_t)_channel, width, &read_raw);
        // *adc2Value = -1;
        //         if (rErr != ESP_OK)
        // *adc2Value = read_raw;
    } else {
        // leave rErr on fault
    }

    if (rErr != ESP_OK)
        ESP_LOGD(cModTag, "Adc channel setup issue %d: %s ", rErr, esp_err_to_name(rErr));

    return read_raw;
}

// void InitAdc2Port(adc2_channel_t ch) {
//     adc2_Port.ch = ch;

//     adc2_Port.status = adc2_pad_get_io_num(adc2_Port.ch, &adc2_Port.port);
//     assert(adc2_Port.status == ESP_OK);

//     ESP_LOGI(cModTag, "ADC2 channel %d setup @GPIO %d ... ", adc2_Port.ch, adc2_Port.port);

//     adc2_Port.status = adc2_config_channel_atten(ch, ADC_ATTEN_11db);
//     // ADC_ATTEN_11db => FullScale = 3.3 V (supply on VDD_2)

//     if (adc2_Port.status != ESP_OK) {
//         ESP_LOGD(cModTag, "ADC2 channel setup issue %d: %s ", adc2_Port.status,
//         esp_err_to_name(adc2_Port.status));
//     }
// }
