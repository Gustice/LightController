/**
 * @file PwmPort.h
 * @author Gustice
 * @brief PWM-class
 * @version 0.1
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include <stdint.h>

/**
 * @brief PWM-Port-Class
 * @details Provides Write to PWM-Port
 */
class PwmPort {
  public:
    PwmPort(ledc_channel_t channel, gpio_num_t output);
    ~PwmPort(){};

    void WritePort(uint32_t duty);

  protected:
    ledc_channel_t _channel;
    gpio_num_t _port;

  private:
    const char *cModTag = "HAL-LedPwm";
    bool _initOk = false;
};
