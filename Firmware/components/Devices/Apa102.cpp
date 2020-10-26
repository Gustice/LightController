/**
 * @file Apa102.cpp
 * @author Gustice
 * @brief SyncLED-class implementation
 * @version 0.1
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Apa102.h"

typedef struct Apa102Color_def {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Bright;
} Apa102Color_t;

const uint8_t BrightnessMask = 0x1Fu;
const uint8_t LedWordStartCode = 0xC0;
const uint32_t LedStreamStartFrame = 0x00000000;
const uint32_t LedStreamEndFrame   = 0xC0000000;

size_t EndCodeLength(size_t ledCnt);

Apa102::Apa102(SpiPort *port, size_t length) {
    _port = port;
    _count = length;
    _brightmess = 0;
    _endCodeLen = EndCodeLength(length);
    // StartFrame + data + StopFrames
    _dataSize = 1+ length + _endCodeLen;
    _ledData = new uint8_t [_dataSize * 4];

    _brightmess = 1;
};

Apa102::~Apa102() { delete[] _ledData; }

size_t EndCodeLength(size_t ledCnt) {
    // Appended clk-stropes = min: LED-Count / 2  => SPI-bits to be transfered as stop signature
    return (size_t)((ledCnt +63) / (2 * 32)); 
    // @todo this must be validated on long stripe (>64 Elements)
}

void Apa102::SetBrightness(uint8_t bright) { _brightmess = bright >> 3u; }

void Apa102::SendImage(Color_t *colors) {
    *((uint32_t *)_ledData) = LedStreamStartFrame;
    uint32_t idx = 4;
    for (size_t i = 0; i < _count; i++) {
        _ledData[idx++] = LedWordStartCode | _brightmess;
        _ledData[idx++] = colors[i].blue;
        _ledData[idx++] = colors[i].green;
        _ledData[idx++] = colors[i].red;
    }
    for (size_t i = idx / 4; i < _dataSize; i++) {
        _ledData[idx++] = (uint8_t)(LedStreamEndFrame >> 24);
        _ledData[idx++] = (uint8_t)(LedStreamEndFrame >> 16);
        _ledData[idx++] = (uint8_t)(LedStreamEndFrame >> 8);
        _ledData[idx++] = (uint8_t)LedStreamEndFrame;
    }

    _port->TransmitSync(_ledData, nullptr, _dataSize * 4);
}
