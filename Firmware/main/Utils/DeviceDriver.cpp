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
const Effect::Sequence StartupLightSequence[] = {
    Effect::Sequence(32, 1, Effect::eEffect::Light_Blank),
    Effect::Sequence(64, 2, Effect::gau8_initSlope, Effect::eEffect::Light_Wave),
    Effect::Sequence(64, 2, Effect::eEffect::Light_Idle), // Ininit Loop
};


const int grayCnt = 16;
const int colorsCnt = 7;
/// Color pool for demo
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
    : EffectCount(4), modTag("DeviceDriver") {
    SLedStrip = sLeds;
    ALedStrip = aLeds;
    LedStrip = ledStrip;
    LedDriver = ledDriver;

    syncPort = new ChannelIndexes(config->SyncLeds.Strip.LedCount, colors, colorsCnt);
    asyncPort = new ChannelIndexes(config->AsyncLeds.Strip.LedCount, colors, colorsCnt);
    rgbPort = new ChannelIndexes(config->RgbStrip.ChannelCount, colors, colorsCnt);
    expander = new uint16_t[config->I2cExpander.Device.LedCount];

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

    uint16_t delays[EffectCount];
    Color_t *startColors[EffectCount];
    startColors[0] = &(config->SyncLeds.Color);
    delays[0] = config->SyncLeds.Delay;
    startColors[1] = &(config->AsyncLeds.Color);
    delays[1] = config->AsyncLeds.Delay;
    startColors[2] = &(config->RgbStrip.Color);
    delays[3] = config->RgbStrip.Delay;

    Color_t cE;
    cE.red = config->I2cExpander.GrayValues[0];
    cE.green = config->I2cExpander.GrayValues[1];
    cE.blue = config->I2cExpander.GrayValues[2];
    startColors[3] = &cE;

    Sequencers = new EffectSequencer *[EffectCount];
    for (size_t i = 0; i < EffectCount; i++) {
        Sequencers[i] = new EffectSequencer(Effect::cu16_TemplateLength, 1, 1);
        Sequencers[i]->SetEffect(
            StartupLightSequence, startColors[i], gu8_idleIntensity, delays[i]);
    }
}

DeviceDriver::~DeviceDriver() {
    delete syncPort;
    delete asyncPort;
    delete rgbPort;
    delete[] expander;

    for (size_t i = 0; i < EffectCount; i++) {
        // delete Sequencers[i];
    }
    // delete[] Sequencers;
}

esp_err_t ApplyColor2RgbChannel(ColorMsg_t *colorMsg, ApplyIndexes_t * apply, ChannelIndexes *port) {
    for (size_t i = 0; i < ApplyToChannelWidth; i++) {
        uint32_t testIdx = 1 << i;
        if ((apply->ApplyTo[0] & testIdx) != 0) {
            if (i >= port->Count)
                return ESP_OK; // Index exceeds strip-size
            port->Image[i].red = colorMsg->red;
            port->Image[i].green = colorMsg->green;
            port->Image[i].blue = colorMsg->blue;
            port->Image[i].white = colorMsg->white;
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

    default:
        break;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DeviceDriver::ReadValue(ReqColorIdx_t channel, uint8_t *data, size_t length) {

    switch (channel.type) {
    case RgbChannel::RgbiSync: {
        ColorMsg_t *cm = (ColorMsg_t *)data;
        Color_t *image = syncPort->Image;
        cm->red = image[channel.portIdx].red;
        cm->green = image[channel.portIdx].green;
        cm->blue = image[channel.portIdx].blue;
        // cm->intensity = sync[channel.portIdx].intensity;
    } break;

    case RgbChannel::RgbwAsync: {
        ColorMsg_t *cm = (ColorMsg_t *)data;
        Color_t *image = asyncPort->Image;
        cm->red = image[channel.portIdx].red;
        cm->green = image[channel.portIdx].green;
        cm->blue = image[channel.portIdx].blue;
    } break;

    case RgbChannel::RgbwPwm: {
        ColorMsg_t *cm = (ColorMsg_t *)data;
        Color_t *image = rgbPort->Image;
        cm->red = image[channel.portIdx].red;
        cm->green = image[channel.portIdx].green;
        cm->blue = image[channel.portIdx].blue;
    } break;

    case RgbChannel::I2cExpanderPwm: {
        GrayValMsg_t *gm = (GrayValMsg_t *)data;

        for (size_t i = 0; i < expLedCount; i++) {
            gm->gray[i] = expander[i];
        }
    } break;

    default:
        ESP_LOGE(modTag, "Channel type not defined");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void DeviceDriver::DemoTick(void) {
    syncPort->ClearImage();
    syncPort->SetNextSlotMindOverflow();

    asyncPort->ClearImage();
    uint16_t aIdx = asyncPort->SetNextSlotMindOverflow();
    SLedStrip->SendImage(syncPort->Image);

    rgbPort->ClearImage();
    rgbPort->SetNextSlotMindOverflow();
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

void SetColorByObject(Color_t *target, Color const *const obj, size_t repeat) {
    Color_t c = obj->GetColor();
    for (size_t i = 0; i < repeat; i++) {
        target[i].red = c.red;
        target[i].green = c.green;
        target[i].blue = c.blue;
        target[i].white = c.white;
    }
}

void DeviceDriver::EffectTick(void) {
    Color const *c1 = Sequencers[0]->Tick();
    Color const *c2 = Sequencers[1]->Tick();
    Color const *c3 = Sequencers[2]->Tick();
    Color const *c4 = Sequencers[3]->Tick();

    SetColorByObject(syncPort->Image, c1, 1);
    SLedStrip->SendImage(syncPort->Image);

    Color c2m = (*c2) * 0x3;
    SetColorByObject(asyncPort->Image, &c2m, 1);
    ALedStrip->SendImage(asyncPort->Image);

    SetColorByObject(rgbPort->Image, c3, 1);
    LedStrip->SetImage(rgbPort->Image);

    Color_t c4s = c4->GetColor();
    expander[0] = (uint16_t)c4s.red << 2;
    expander[1] = (uint16_t)c4s.green << 2;
    expander[3] = (uint16_t)c4s.blue << 2;

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