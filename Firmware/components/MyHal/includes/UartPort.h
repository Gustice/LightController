/**
 * @file UartPort.h
 * @author Gustice
 * @brief UART-Port class
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include "PortBase.h"
#include "driver/gpio.h"
#include "driver/uart.h"

/**
 * @brief UART-Port Class
 * @details Provides Write and Read operations via UART-Interface
 */
class UartPort : PortBase {
  public:
    UartPort(gpio_num_t txd, gpio_num_t rxd);
    ~UartPort(){};

    esp_err_t TransmitSync(const uint8_t *txData, size_t len);
    int ReceiveSync(uint8_t *txData, size_t len);

  private:
    int _uartH;
};
