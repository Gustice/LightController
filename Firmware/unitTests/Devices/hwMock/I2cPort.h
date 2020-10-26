#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <string.h>

class I2cPort {
  public:
    I2cPort(){};
    ~I2cPort(){};

    uint8_t SendBuffer[512];
    uint8_t ReceiveBuffer[512];
    size_t LastLen;

    esp_err_t WriteData(uint16_t slaveAdd, uint8_t *data, size_t size) {
        LastLen = size;
        // UNSCOPED_INFO("Writing " << size << " bytes to I2C-Slave " << slaveAdd);
        for (size_t i = 0; i < size; i++) {
            SendBuffer[i] = data[i];
        }
        return ESP_OK;
    }
    esp_err_t ReadData(uint16_t slaveAdd, uint8_t *data, size_t size) {
        // UNSCOPED_INFO("Reading " << size << " bytes to I2C-Slave " << slaveAdd);
        for (size_t i = 0; i < size; i++) {
            data[i] = ReceiveBuffer[i];
        }
        return ESP_OK;
    }
};

