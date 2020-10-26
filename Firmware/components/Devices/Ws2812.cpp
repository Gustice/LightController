/**
 * @file Ws2812.cpp
 * @author Gustice
 * @brief AsyncLED-class implementation
 * @version 0.1
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Ws2812.h"

typedef struct Ws2812Color_def {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} Ws2812Color_t;

typedef struct Sk6812Color_def {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t White;
} Sk6812Color_t;

Ws2812::Ws2812(RmtPort * port, uint16_t length) {
    _port = port;
    _count = length;
    _ledData = new uint8_t[_count * 4];
};

Ws2812::~Ws2812() { delete[] _ledData; }

void Ws2812::SendImage(Color_t *colors) {
    uint32_t idx = 0;
    for (size_t i = 0; i < _count; i++) {
        _ledData[idx++] = colors[i].green;
        _ledData[idx++] = colors[i].red;
        _ledData[idx++] = colors[i].blue;
        // _ledData[idx++] = colors[i].white;
    }

    _port->WriteData(_ledData, _count * 3);
}

// esp_err_t @todo Prepare Log-Function in all Hardware Abstraction objects