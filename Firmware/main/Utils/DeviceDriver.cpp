/**
 * @file DeviceDriver.h
 * @author Gustice
 * @brief Implementation of Driver
 * @version 0.1
 * @date 2021-03-04
 *
 * @copyright Copyright (c) 2021
 */

#include "DeviceDriver.h"
#include "Color.h"
#include "RotatingIndex.h"
#include "SequenceStepper.h"
#include "esp_log.h"
#include <string.h>

const int grayCnt = 16;
/// Color pool for demo
const Color_t *colors[]{
    &color_Red,
    &color_Green,
    &color_Blue,
    &color_Cyan,
    &color_Magenta,
    &color_Yellow,
    &color_White,
};
const int colorsCnt = sizeof(colors) / sizeof(Color_t);

DeviceDriver::DeviceDriver(Apa102 *sLeds, Ws2812 *aLeds, RgbwStrip *ledStrip, Pca9685 *ledDriver,
    EffectComplex *effects, deviceConfig_t *config)
    : modTag("DeviceDriver") {
    SLedStrip = sLeds;
    ALedStrip = aLeds;
    LedStrip = ledStrip;
    LedDriver = ledDriver;
    Effects = effects;

    syncPort = new ChannelIndexes(config->SyncLeds.Strip.LedCount, colors, colorsCnt);
    asyncPort = new ChannelIndexes(config->AsyncLeds.Strip.LedCount, colors, colorsCnt);
    rgbPort = new ChannelIndexes(config->RgbStrip.ChannelCount, colors, colorsCnt);
    expander = new uint16_t[config->I2cExpander.Device.LedCount];

    for (size_t i = 0; i < (int)RgbChannel::LastChannel; i++) {
        Images[i] = nullptr;
    }

    Images[RgbChannel::RgbiSync] = syncPort;
    Images[RgbChannel::RgbwAsync] = asyncPort;
    Images[RgbChannel::RgbwPwm] = rgbPort;

    syncPort->ClearImage();
    asyncPort->ClearImage();
    rgbPort->ClearImage();
    expLedCount = config->I2cExpander.Device.LedCount;
    memset(expander, 0, sizeof(uint16_t) * config->I2cExpander.Device.LedCount);

    SLedStrip->SetBrightness(0x20);
    demoTickCount = 0;

    SLedStrip->SendImage(syncPort->Image);
    ALedStrip->SendImage(asyncPort->Image);
    LedStrip->SetImage(rgbPort->Image);
    LedDriver->SendImage(expander);

    Color_t *startColors[4];
    startColors[0] = &(config->SyncLeds.Color);
    startColors[1] = &(config->AsyncLeds.Color);
    startColors[2] = &(config->RgbStrip.Color);

    Color_t cE;
    cE.red = config->I2cExpander.GrayValues[0];
    cE.green = config->I2cExpander.GrayValues[1];
    cE.blue = config->I2cExpander.GrayValues[2];
    startColors[3] = &cE;

    Config = config;
    if (config->StartUpMode == DeviceStartMode::StartImage) {
        effects->ActivateStartupEffects();
    }
}

DeviceDriver::~DeviceDriver() {
    delete syncPort;
    delete asyncPort;
    delete rgbPort;
    delete[] expander;
}

esp_err_t ApplyColor2RgbChannel(ColorMsg_t *colorMsg, ApplyIndexes_t *apply, ChannelIndexes *port) {
    for (size_t i = 0; i < ApplyToChannelWidth; i++) {
        uint32_t testIdx = 1 << i;
        if ((apply->ApplyTo[0] & testIdx) != 0) {
            if (i >= port->Count)
                return ESP_OK; // Index exceeds strip-size
            CopyColorMessageToColor(&port->Image[i], colorMsg);
        }
    }
    return ESP_OK;
}

esp_err_t DeviceDriver::SetValue(
    ReqColorIdx_t channel, uint8_t *data, size_t length, ApplyIndexes_t *apply) {
    ColorMsg_t *cm = (ColorMsg_t *)data;
    GrayValMsg_t *gm = (GrayValMsg_t *)data;

    switch (channel.type) {
    case RgbChannel::RgbiSync:
        ApplyColor2RgbChannel(cm, apply, syncPort);
        SLedStrip->SendImage(syncPort->Image);
        break;

    case RgbChannel::RgbwAsync:
        ApplyColor2RgbChannel(cm, apply, asyncPort);
        ALedStrip->SendImage(asyncPort->Image);
        break;

    case RgbChannel::RgbwPwm:
        ApplyColor2RgbChannel(cm, apply, rgbPort);
        LedStrip->SetImage(rgbPort->Image);
        break;

    case RgbChannel::I2cExpanderPwm: {
        for (size_t i = 0; i < expLedCount; i++) {
            expander[i] = (uint16_t)gm->gray[i] << 4;
        }
        LedDriver->SendImage(expander);
    } break;

    case RgbChannel::EffectProcessor: {
        Color_t c = {.red = cm->red, .green = cm->green, .blue = cm->blue, .white = cm->white};
        Effects->FadeToColor(channel.portIdx, c);
    } break;

    default:
        break;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DeviceDriver::ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length) {
    int LinearIdx = ApplyToChannelWidth * channel.chIdx + channel.portIdx;

    switch (channel.type) {
    case RgbChannel::RgbiSync:
        if (LinearIdx >= syncPort->Count)
            return ESP_FAIL;
        CopyColorToColorMessage((ColorMsg_t *)data, &syncPort->Image[LinearIdx]);
        break;
    case RgbChannel::RgbwAsync:
        if (LinearIdx >= asyncPort->Count)
            return ESP_FAIL;
        CopyColorToColorMessage((ColorMsg_t *)data, &asyncPort->Image[LinearIdx]);
        break;
    case RgbChannel::RgbwPwm:
        if (LinearIdx >= rgbPort->Count)
            return ESP_FAIL;
        CopyColorToColorMessage((ColorMsg_t *)data, &rgbPort->Image[LinearIdx]);
        break;

    case RgbChannel::I2cExpanderPwm: {
        GrayValMsg_t *gm = (GrayValMsg_t *)data;

        for (size_t i = 0; i < expLedCount; i++) {
            gm->gray[i] = expander[i];
        }
    } break;

    case RgbChannel::EffectProcessor: {
        if (LinearIdx >= Effects->Count)
            return ESP_FAIL;
        Color_t c;
        Effects->ReadColor(channel.portIdx, c);
        CopyColorToColorMessage((ColorMsg_t *)data, &c);
        break;
    }

    default:
        ESP_LOGE(modTag, "Channel type not defined");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void DeviceDriver::DemoTick(void) {
    syncPort->ClearImage();
    syncPort->GetNextSlot();

    asyncPort->ClearImage();
    uint16_t aIdx = asyncPort->GetNextSlot();
    SLedStrip->SendImage(syncPort->Image);

    rgbPort->ClearImage();
    rgbPort->GetNextSlot();
    // todo this is shit ... implement maximum brightness
    asyncPort->Image[aIdx].red = asyncPort->Image[aIdx].red >> 5;
    asyncPort->Image[aIdx].green = asyncPort->Image[aIdx].green >> 5;
    asyncPort->Image[aIdx].blue = asyncPort->Image[aIdx].blue >> 5;
    asyncPort->Image[aIdx].white = asyncPort->Image[aIdx].white >> 5;

    ALedStrip->SendImage(asyncPort->Image);
    LedStrip->SetImage(rgbPort->Image);

    uint16_t pattern = demoTickCount;
    for (size_t i = 0; i < expLedCount; i++) {
        expander[i] = 0;
        if ((pattern & (1 << i)) != 0) {
            expander[i] = (uint16_t)0x200;
        }
    }
    LedDriver->SendImage(expander);
    demoTickCount++;
}

void SetColorByObject(Color_t *target, Color const *const obj, size_t index) {
    Color_t c = obj->GetColor();
    target[index].red = c.red;
    target[index].green = c.green;
    target[index].blue = c.blue;
    target[index].white = c.white;
}

void DeviceDriver::EffectTick(void) {
    for (size_t i = 0; i < Effects->Count; i++) {
        effectProcessor_t *config = &(Config->EffectMachines[i]);
        if (config->Target != TargetGate::None) {
            Color const *color = Effects->TickEffect(i);
            Color_t *image = nullptr;
            switch (config->Target) {
            case TargetGate::SyncLed:
                image = syncPort->Image;
                break;
            case TargetGate::AsyncLed:
                image = asyncPort->Image;
                break;
            case TargetGate::LedStrip:
                image = rgbPort->Image;
                break;
            case TargetGate::I2cExpander: /*@todo*/
                break;
            default:
                image = nullptr;
            }

            if (image != nullptr) {
                uint32_t apply = config->ApplyFlags;
                for (size_t i = 0; i < 32; i++) {
                    if ((((uint32_t)1 << i) & apply) != 0) {
                        SetColorByObject(image, color, i);
                    }
                }
            }
        }
    }

    SLedStrip->SendImage(syncPort->Image);
    ALedStrip->SendImage(asyncPort->Image);
    LedStrip->SetImage(rgbPort->Image);

    uint16_t pattern = demoTickCount;
    for (size_t i = 0; i < expLedCount; i++) {
        expander[i] = 0;
        if ((pattern & (1 << i)) != 0) {
            expander[i] = (uint16_t)0x200;
        }
    }
    LedDriver->SendImage(expander);
    demoTickCount++;
}