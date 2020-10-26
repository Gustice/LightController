/**
 * @file SpiPort.h
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
#include "driver/spi_master.h"

typedef enum {
    mode0 = 0,
    mode1 = 1,
    mode2 = 2,
    mode3 = 3,
} spi_mode_t;

/**
 * @brief SPI-Port Class
 * @details Provides Write and Read operations via SPI-Interface
 */
class SpiPort : public PortBase {
  public:
    enum SpiPorts {
        /* ist not a good idea to use SPI1_HOST*/
        HSpi = SPI2_HOST,
        VSpi = SPI3_HOST,
    };

    SpiPort(SpiPorts interface, spi_mode_t mode, gpio_num_t csPort = gpio_num_t::GPIO_NUM_NC);
    SpiPort(SpiPorts interface, gpio_num_t mosi, gpio_num_t miso, gpio_num_t clk, spi_mode_t mode,
            gpio_num_t csPort = gpio_num_t::GPIO_NUM_NC);
    ~SpiPort(){};

    esp_err_t TransmitSync(const uint8_t *txData, uint8_t *rxData, size_t len);

  private:
    spi_device_handle_t _spiH;
};
