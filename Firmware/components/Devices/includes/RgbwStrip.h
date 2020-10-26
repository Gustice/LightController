/**
 * @file RgbwStrip.h
 * @author Gustice
 * @brief Rgbw-LED-Strips-Class
 * @version 0.1
 * @date 2020-10-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include "Color.h"
#include "PwmPort.h"

/**
 * @brief LED-Class for RGB(W) LED-Strips
 */
class RgbwStrip {
  private:
    PwmPort * _r;
    PwmPort * _g;
    PwmPort * _b;
    PwmPort * _w;
    
  public:
    RgbwStrip(PwmPort * red, PwmPort * green, PwmPort * blue, PwmPort * white = nullptr);
    ~RgbwStrip(){};

    void SetImage(const Color_t *color);
};
