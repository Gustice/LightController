#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <string.h>

class RmtPort {
  public:
    enum RmtProtocols {
        RtzWs2812 = 0,
        Phillips,
        Undefined,
    };

    RmtPort(){};
    ~RmtPort(){};

    uint8_t SendBuffer[512];
    uint8_t ReceiveBuffer[512];
    size_t LastLen;

    esp_err_t WriteData(uint8_t *data, uint16_t length) {
        LastLen = length;
        // UNSCOPED_INFO("Writing " << length << " bytes to RMT-Port");
        for (size_t i = 0; i < length; i++) {
            SendBuffer[i] = data[i];
        }
        return ESP_OK;
    }
    RmtProtocols GetProtocol(void) { return Undefined; }
};
