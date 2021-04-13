/**
 * @file DeviceDriver.h
 * @author Gustice
 * @brief Driving the lighting-ports
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 */
#pragma once

#include "Apa102.h"
#include "Color.h"
#include "ApiDataTypes.h"
#include "ParamReader.h"
#include "Pca9685.h"
#include "RgbwStrip.h"
#include "Ws2812.h"
#include "ChannelIndexes.h"

/**
 * @brief Accumulated device-function regarding the lighting.
 * 
 */
class DeviceDriver {
  public:
    DeviceDriver(Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver,
        deviceConfig_t *config);
    ~DeviceDriver();

    esp_err_t ApplyRgbColorMessage(ColorMsg_t *colorMsg);
    esp_err_t ApplyGrayValueMessage(GrayValMsg_t *GrayMsg);

    /**
     * @brief Function for read-requests concering the applied color data
     * 
     * @param channel target of the read operation. Also defines data format
     * @param data data stream to write back requested values
     * @param length size of the provided container
     * @return esp_err_t 
     */
    esp_err_t ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length);

    void DemoTick(void);

    esp_err_t ApplyColorToWholeChannel(Color_t color, RgbChannel channel);

  private:
    const char *modTag;
    Apa102 *SLedStrip;
    Ws2812 *ALedStrip;
    Pca9685 *LedDriver;
    RgbwStrip *LedStrip;

    ChannelIndexes * syncPort;
    ChannelIndexes * asyncPort;
    ChannelIndexes * rgbPort;
    uint16_t *expander;
    uint16_t expLedCount;

    uint16_t demoTickCount;
};
