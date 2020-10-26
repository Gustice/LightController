/**
 * @file I2cPort.h
 * @author Gustice
 * @brief I2C-Port class
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "PortBase.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

/**
 * @brief I2C-Port Class
 * @details Provides Write and Read operations via I2C-Interface
 */
class I2cPort : public PortBase {
  public:
    enum I2cInterfaces {
        Int1 = 0,
        Int2,
    };

    I2cPort(i2c_port_t port);
    I2cPort(i2c_port_t port, gpio_num_t sda, gpio_num_t scl);
    ~I2cPort(){};

    esp_err_t WriteData(uint16_t slaveAdd, uint8_t *data, size_t size);
    esp_err_t ReadData(uint16_t slaveAdd, uint8_t *data, size_t size);

  private:
    i2c_port_t _port;
};
