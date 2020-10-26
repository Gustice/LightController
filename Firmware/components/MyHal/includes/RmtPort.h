/**
 * @file RmtPort.h
 * @author Gustice
 * @brief
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "PortBase.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

/**
  * @brief RMT-Port Class
 * @details Provides Write and Read operations via RMT-Interface (Infrared Remote Controls)
 */
class RmtPort : public PortBase {
  public:
    enum RmtProtocols {
        RtzWs2812 = 0,
        Phillips,
        Undefined,
    };

    RmtPort(rmt_channel_t channel, gpio_num_t pin, RmtProtocols protocol);
    ~RmtPort(){};

    esp_err_t WriteData(uint8_t *data, uint16_t length);
    RmtProtocols GetProtocol(void) { return _protocol; }

  private:
    rmt_channel_t _channel;
    RmtProtocols _protocol = RmtProtocols::Undefined;
    uint32_t _timeout;
    size_t _lastLen = 0;
};
