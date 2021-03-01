#pragma once

#include "Apa102.h"
#include "Color.h"
#include "ApiDataTypes.h"
#include "ParamReader.h"
#include "Pca9685.h"
#include "RgbwStrip.h"
#include "Ws2812.h"
#include "ChannelIndexes.h"

class DeviceDriver {
  public:
    DeviceDriver(Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver,
        deviceConfig_t *config);
    ~DeviceDriver();

    esp_err_t ApplyConfiguration(deviceConfig_t *config);

    esp_err_t ApplyRgbColorMessage(ColorMsg_t *colorMsg);
    esp_err_t ApplyGrayValueMessage(GrayValMsg_t *GrayMsg);
    esp_err_t ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length);

    void DemoTick(void);
    void SetupDemo(void);

  private:
    const char *modTag;
    Apa102 *SLedStrip;
    Ws2812 *ALedStrip;
    Pca9685 *LedDriver;
    RgbwStrip *LedStrip;
    deviceConfig_t *deviceConfig;

    Color_t *sync;
    Color_t *async;
    Color_t *strip;
    uint16_t *expander;

    ChannelIndexes * syncPort;
    ChannelIndexes * asyncPort;
    ChannelIndexes * rgbPort;
    uint16_t demoTickCount;
    bool setupOk;
};
