/**
 * @file DacPort.h
 * @author Gustice
 * @brief DAC-Port class
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "PortBase.h"
#include "driver/dac.h"
#include "driver/gpio.h"

/**
 * @brief Digital-to-Analogue-Converter-Port Class
 * @details Provides Write on analog ports
 */
class DacPort : public PortBase {
  public:
    DacPort(dac_channel_t channel);
    ~DacPort();

    void WritePort(uint8_t value);

  protected:
    gpio_num_t _port;
    dac_channel_t _channel;
};
