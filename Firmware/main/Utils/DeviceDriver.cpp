
#include "DeviceDriver.h"
#include "Color.h"
#include "RotatingIndex.h"
#include <string.h>
#include "esp_log.h"

const int grayCnt = 16;
const int colorsCnt = 7;
const Color_t *colors[colorsCnt]{
    &color_Red,
    &color_Green,
    &color_Blue,
    &color_Cyan,
    &color_Magenta,
    &color_Yellow,
    &color_White,
};

DeviceDriver::DeviceDriver(
    Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver, deviceConfig_t *config)
    : modTag("DeviceDriver") {
    SLedStrip = sLeds;
    ALedStrip = aLeds;
    LedStrip = ledStrip;
    LedDriver = ledDriver;
    deviceConfig = config;

    syncPort = new ChannelIndexes(config->SyncLeds.Strip.LedCount, colors, colorsCnt);
    asyncPort = new ChannelIndexes(config->AsyncLeds.Strip.LedCount, colors, colorsCnt);
    rgbPort = new ChannelIndexes(config->RgbStrip.ChannelCount, colors, colorsCnt);
    expander = new uint16_t[deviceConfig->I2cExpander.Device.LedCount];

    syncPort->ClearImage();
    asyncPort->ClearImage();
    rgbPort->ClearImage();
    memset(expander, 0, sizeof(uint16_t) * deviceConfig->I2cExpander.Device.LedCount);

    SLedStrip->SetBrightness(0x20);
    setupOk = true;
    demoTickCount = 0;

    SLedStrip->SendImage(syncPort->Image);
    ALedStrip->SendImage(asyncPort->Image);
    LedStrip->SetImage(rgbPort->Image);
    LedDriver->SendImage(expander);
}

DeviceDriver::~DeviceDriver() {
    delete syncPort;
    delete asyncPort;
    delete rgbPort;
    delete[] expander;
}

esp_err_t ApplyColor2RgbChannel(ColorMsg_t *colorMsg, Color_t *colors, size_t size) {
    for (size_t i = 0; i < ApplyToChannelWidth; i++) {
        uint32_t testIdx = 1 << i;
        if ((colorMsg->apply.ApplyTo[0] & testIdx) != 0) {
            if (i >= size)
                return ESP_OK; // Index exceeds strip-size
            colors[i].red = colorMsg->red;
            colors[i].green = colorMsg->green;
            colors[i].blue = colorMsg->blue;
            colors[i].white = colorMsg->white;
        }
    }
    return ESP_OK;
}

esp_err_t DeviceDriver::ApplyRgbColorMessage(ColorMsg_t *colorMsg) {
    if (deviceConfig == nullptr)
        return ESP_FAIL;

    switch (colorMsg->channel) {
    case RgbChannel::RgbiSync:
        ApplyColor2RgbChannel(colorMsg, syncPort->Image, deviceConfig->SyncLeds.Strip.LedCount);
        SLedStrip->SendImage(syncPort->Image);
        break;

    case RgbChannel::RgbwAsync:
        ApplyColor2RgbChannel(colorMsg, asyncPort->Image, deviceConfig->AsyncLeds.Strip.LedCount);
        ALedStrip->SendImage(asyncPort->Image);
        break;

    case RgbChannel::RgbwPwm:
        ApplyColor2RgbChannel(colorMsg, rgbPort->Image, deviceConfig->RgbStrip.ChannelCount);
        ALedStrip->SendImage(rgbPort->Image);
        break;

    default:
        break;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DeviceDriver::ApplyGrayValueMessage(GrayValMsg_t *GrayMsg) {
    if (deviceConfig == nullptr)
        return ESP_FAIL;

    for (size_t i = 0; i < deviceConfig->I2cExpander.Device.LedCount; i++) {
        expander[i] = (uint16_t)GrayMsg->gray[i] << 4;
    }
    LedDriver->SendImage(expander);
    return ESP_OK;
}

esp_err_t DeviceDriver::ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length) {
    if (deviceConfig == nullptr)
        return ESP_FAIL;

    ColorMsg_t *cm = (ColorMsg_t *)data;

    switch (channel.type) {
    case RgbChannel::RgbiSync: {
        Color_t * image = syncPort->Image;
        cm->red = image[cm->apply.FirstTarget.portIdx].red;
        cm->green = image[cm->apply.FirstTarget.portIdx].green;
        cm->blue = image[cm->apply.FirstTarget.portIdx].blue;
        // cm->intensity = sync[cm->apply.FirstTarget.portIdx].intensity;
    } break;

    case RgbChannel::RgbwAsync: {
        Color_t * image = asyncPort->Image;
        cm->red = image[cm->apply.FirstTarget.portIdx].red;
        cm->green = image[cm->apply.FirstTarget.portIdx].green;
        cm->blue = image[cm->apply.FirstTarget.portIdx].blue;
    } break;

    case RgbChannel::RgbwPwm: {
        Color_t * image = rgbPort->Image;
        cm->red = image[cm->apply.FirstTarget.portIdx].red;
        cm->green = image[cm->apply.FirstTarget.portIdx].green;
        cm->blue = image[cm->apply.FirstTarget.portIdx].blue;
    } break;

    case RgbChannel::I2cExpanderPwm: {
        GrayValMsg_t *gm = (GrayValMsg_t *)data;
        for (size_t i = 0; i < deviceConfig->I2cExpander.Device.LedCount; i++) {
            gm->gray[i] = expander[i];
        }
    } break;

    default:
        ESP_LOGE(modTag, "Channel type not defined");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void DeviceDriver::SetupDemo(void) {
    if (deviceConfig == nullptr)
        return;

}

void DeviceDriver::DemoTick(void) {
    syncPort->ClearImage();
    syncPort->SetNextSlotMindOverflow();

    asyncPort->ClearImage();
    uint16_t aIdx = asyncPort->SetNextSlotMindOverflow();

    rgbPort->ClearImage();
    rgbPort->SetNextSlotMindOverflow();

    // todo this is shit ... implement maximum brightness
    asyncPort->Image[aIdx].red = asyncPort->Image[aIdx].red >> 5;
    asyncPort->Image[aIdx].green = asyncPort->Image[aIdx].green >> 5;
    asyncPort->Image[aIdx].blue = asyncPort->Image[aIdx].blue >> 5;
    asyncPort->Image[aIdx].white = asyncPort->Image[aIdx].white >> 5;

    SLedStrip->SendImage(syncPort->Image);
    ALedStrip->SendImage(asyncPort->Image);
    LedStrip->SetImage(rgbPort->Image);

    uint16_t pattern = demoTickCount;
    for (size_t i = 0; i < deviceConfig->I2cExpander.Device.LedCount; i++) {
        expander[i] = 0;
        if ((pattern & (1 << i)) != 0) {
            expander[i] = (uint16_t)0x200;
        }
    }
    LedDriver->SendImage(expander);
    demoTickCount++;
}
