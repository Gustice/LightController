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
#include "Sequencer.h"

using namespace Effect;

/**
 * @brief Accumulated device-function regarding the lighting.
 * 
 */
class DeviceDriver {
  public:
    DeviceDriver(Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver,
        deviceConfig_t *config);
    ~DeviceDriver();

    /**
     * @brief Function for write-requests concering the applied data
     * 
     * @param channel target of the read operation. Also defines data format
     * @param data data stream to write back requested values
     * @param length size of the provided container
     * @param apply bit array that specifies targets for the new value
     * @return esp_err_t 
     */
    esp_err_t SetValue(ReqColorIdx_t channel, uint8_t *data, size_t length, ApplyIndexes_t * apply);

    /**
     * @brief Function for read-requests concering the applied values
     * 
     * @param channel target of the read operation. Also defines data format
     * @param data data stream to write back requested values
     * @param length size of the provided container
     * @return esp_err_t 
     */
    esp_err_t ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length);

    void DemoTick(void);
    void EffectTick(void);
    const size_t EffectCount;

  private:
    const char *modTag;
    Apa102 *SLedStrip;
    Ws2812 *ALedStrip;
    Pca9685 *LedDriver;
    RgbwStrip *LedStrip;
    EffectSequencer ** Sequencers;

    ChannelIndexes * syncPort;
    ChannelIndexes * asyncPort;
    ChannelIndexes * rgbPort;
    uint16_t *expander;
    uint16_t expLedCount;

    uint16_t demoTickCount;
    deviceConfig_t * Config;
};
