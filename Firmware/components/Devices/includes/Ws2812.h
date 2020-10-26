/**
 * @file Ws2812.h
 * @author Gustice
 * @brief AsyncSerial LED-Strips-Class
 * @version 0.1
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include "Color.h"
#include "RmtPort.h"

/**
 * @brief LED-Class for AsyncSerial LED-Strips
 * @details Covert Types are: WS2812B, WS2813, SK6812, SK6805, LC8805, LC8808, LC8812, also RGBW-Types like SK6812
 */
class Ws2812 {
  private:
    RmtPort * _port;
    size_t _count;
    uint8_t * _ledData;

  public:
    Ws2812(RmtPort * port, uint16_t length);
    ~Ws2812();

    void SendImage(Color_t *colors);
};
