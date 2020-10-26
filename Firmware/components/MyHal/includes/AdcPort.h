/**
 * @file AdcPort.h
 * @author Gustice
 * @brief ADC-Port class
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "PortBase.h"
#include "driver/adc.h"
#include "driver/gpio.h"

/**
 * @brief Analogue-to-Digital-Converter-Port Class
 * @details Provides Read on analog ports
 */
class AdcPort : public PortBase {
  public:
    AdcPort(adc1_channel_t channel);
    ~AdcPort(){};

    uint16_t ReadPort(void);

  protected:
    adc_channel_t _channel;
    gpio_num_t _port;
    adc_unit_t _unit;
};
