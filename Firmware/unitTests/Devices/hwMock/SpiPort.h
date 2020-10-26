#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <string.h>

class SpiPort {
  public:
    SpiPort(){};
    ~SpiPort(){};

    uint8_t SendBuffer[512];
    uint8_t ReceiveBuffer[512];
    size_t LastLen;

    esp_err_t TransmitSync(const uint8_t *txData, uint8_t *rxData, size_t len) {
        LastLen = len;
        // UNSCOPED_INFO("Writing data");

        assert(len <= sizeof(SendBuffer));
        for (size_t i = 0; i < len; i++) {
            SendBuffer[i] = txData[i];
        }

        return ESP_OK;
    }
};
