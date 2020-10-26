/**
 * @file Apa102.h
 * @author Gustice
 * @brief SyncSerial LED-Strips-Class
 * @version 0.1
 * @date 2020-10-12
 *
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "Color.h"
#include "SpiPort.h"
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief LED-Class for SyncSerial LED-Strips
 * @details Covert Types are: APA102C, SK9822, LC8823, LC8822
 */
class Apa102 {
  private:
    SpiPort *_port;
    size_t _count;
    size_t _dataSize;
    int8_t _brightmess;
    uint8_t *_ledData;
    size_t _endCodeLen;

  public:
    Apa102(SpiPort *port, size_t length);
    ~Apa102();

    void SendImage(Color_t *colors);
    void SetBrightness(uint8_t bright);
};
