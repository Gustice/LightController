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
#include "ApiDataTypes.h"
#include "ChannelIndexes.h"
#include "Color.h"
#include "EffectComplex.h"
#include "ParamReader.h"
#include "Pca9685.h"
#include "RgbwStrip.h"
#include "Ws2812.h"

using namespace Effect;


/**
 * @brief Accumulated device-function regarding the lighting.
 */
class DeviceDriver {
  public:
    /**
     * @brief Construction of device driver
     * @param sLeds Reference to Sync-LED-Driver
     * @param aLeds Reference to Async-LED-Driver
     * @param ledStrip Reference to RGB-LED-Driver
     * @param ledDriver Reference to LED-Port-Expander
     * @param effects Reference to Effect-Channels
     * @param config Reference to configuration
     */
    DeviceDriver(Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver,
        EffectComplex *effects, deviceConfig_t *config);
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
    esp_err_t SetValue(ReqColorIdx_t channel, uint8_t *data, size_t length, ApplyIndexes_t *apply);

    /**
     * @brief Function for read-requests concering the applied values
     *
     * @param channel target of the read operation. Also defines data format
     * @param data data stream to write back requested values
     * @param length size of the provided container
     * @return esp_err_t
     */
    esp_err_t ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length);

    /**
     * @brief Tick for demo program
     * @brief This demo iterates possible colers over each available pixel
     */
    void DemoTick(void);

    /**
     * @brief Tick for effect mode
     * @details In effect mode the pixels are not just set, but driven by the
     *  effect processor which enables smooth switching between color schemes.
     */
    void EffectTick(void);

    /// Reference to Image handles (for manual modification of colors)
    ChannelIndexes *Images[RgbChannel::LastChannel];

  private:
    /// Module tag for logging
    const char *modTag;

    Apa102 *SLedStrip;
    Ws2812 *ALedStrip;
    Pca9685 *LedDriver;
    RgbwStrip *LedStrip;
    EffectComplex *Effects;
    ChannelIndexes *syncPort;
    ChannelIndexes *asyncPort;
    ChannelIndexes *rgbPort;
    uint16_t *expander;
    uint16_t expLedCount;
    uint16_t demoTickCount;


    /// Reference to device configuration
    deviceConfig_t *Config;
};
