/**
 * @file RmtPort.cpp
 * @author Gustice
 * @brief Rmt-Port classes implementation
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#include "iRmtInclude.h"

typedef struct RtzProtocols_def {
    uint32_t nsTicks_t0h;
    uint32_t nsTicks_t0l;
    uint32_t nsTicks_t1h;
    uint32_t nsTicks_t1l;
    uint32_t nsTicks_Reset;
} RtzProtocols_t;

RtzProtocols_t RtzProtocols[] = {
    {350, 1000, 1000, 350, 280000},
};

static uint32_t t0h_ticks = 0;
static uint32_t t1h_ticks = 0;
static uint32_t t0l_ticks = 0;
static uint32_t t1l_ticks = 0;
static uint32_t bytePeriode_ms = 0;

// todo move this adaptor outside of class
static void IRAM_ATTR ws2812_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
                                         size_t wanted_num, size_t *translated_size,
                                         size_t *item_num) {
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{t0h_ticks, 1, t0l_ticks, 0}}}; // Logical 0
    const rmt_item32_t bit1 = {{{t1h_ticks, 1, t1l_ticks, 0}}}; // Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pDest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pDest->val = bit1.val;
            } else {
                pDest->val = bit0.val;
            }
            num++;
            pDest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

esp_err_t RmtPort::WriteData(uint8_t *data, uint16_t length) {
    if (InitOk != true)
        return ESP_FAIL;

    if (length != _lastLen) {
        _lastLen = length;
        _timeout = bytePeriode_ms * length;
        ESP_LOGI(cModTag, "New Timeout set to : %d ms", _timeout);
    }

    esp_err_t ret = rmt_write_sample(_channel, data, length, true);

    if (ret != ESP_OK) {
        ESP_LOGE(cModTag, "transmit RMT samples failed: ");
        return ret;
    }

    return rmt_wait_tx_done(_channel, pdMS_TO_TICKS(_timeout));
}

RmtPort::RmtPort(rmt_channel_t channel, gpio_num_t pin, RmtProtocols protocol)
    : PortBase("HAL-Rmt") {
    if (channel >= RMT_CHANNEL_MAX) {
        ESP_LOGE(cModTag, "rmt port out of range");
        return;
    }
    if (protocol >= Phillips) {
        ESP_LOGE(cModTag, "protocol out of range");
        return;
    }

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel);
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    int idx = (int)protocol;
    _protocol = protocol;
    _channel = channel;

    uint32_t counter_clk_hz = 0;
    if (rmt_get_counter_clock(channel, &counter_clk_hz) != ESP_OK) {
        ESP_LOGE(cModTag, "get rmt counter clock failed: ");
        return;
    }

    // ns -> ticks
    float ratio = (float)counter_clk_hz / 1e9f;
    t0h_ticks = (uint32_t)(ratio * RtzProtocols[idx].nsTicks_t0h);
    t0l_ticks = (uint32_t)(ratio * RtzProtocols[idx].nsTicks_t0l);
    t1h_ticks = (uint32_t)(ratio * RtzProtocols[idx].nsTicks_t1h);
    t1l_ticks = (uint32_t)(ratio * RtzProtocols[idx].nsTicks_t1l);
    bytePeriode_ms =
        ((uint32_t)RtzProtocols[idx].nsTicks_t0h + RtzProtocols[idx].nsTicks_t0l) * 8 / 1000 + 1;

    if (rmt_translator_init(channel, ws2812_rmt_adapter)) {
        ESP_LOGE(cModTag, "get rmt init failed: ");
        return;
    }

    ESP_LOGV(cModTag, "Rmt Port %d Pin %d initialized ...", channel, pin);
    InitOk = true;
}
