/**
 * @file DeviceTypes.h
 * @author Gustice
 * @brief General parameter definitions
 * @version 0.1
 * @date 2021-03-04
 *
 * @copyright Copyright (c) 2021
 */
#pragma once

#include "Color.h"

typedef struct factoryInfo_def { // @todo the container can be easily overwritten
    char DeviceType[24];
    char SerialNumber[12]; // Something like SN:123ABC
    char HwVersion[16];    // V 1.00.00
    char SwVersion[12];
} factoryInfo_t;

typedef struct wifiConfig_def {
    uint8_t ssid[32];
    /** @note Length must be >= 8 (if not 0) */
    uint8_t password[64];
    uint8_t max_connection;
} WifiConfig_t;

typedef enum ColorChannels {
    NoChannel = 0,
    RGBI,
    RGB,
    RGBW,
    Gray,
} ColorChannels_t;

enum DeviceStartMode {
    StartDark,
    RunDemo,
    StartImage,
};

struct stripConfig_t {
    uint16_t LedCount;
    uint16_t Intensity;
    ColorChannels_t Channels;
};

struct syncLedConfig_t {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
    uint16_t Delay;
};

struct aSyncLedConfig_t {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
    uint16_t Delay;
};

struct rgbwLedConfig_t {
    bool IsActive;
    stripConfig_t Strip;
    uint16_t ChannelCount;
    Color_t Color;
    uint16_t Delay;
};

struct i2cExpander_t {
    bool IsActive;
    stripConfig_t Device;
    uint16_t Address;
    uint16_t GrayValues[16];
};

enum TargetGate {
    None = 0,
    SyncLed,
    AsyncLed,
    LedStrip,
    I2cExpander,
};

struct effectProcessor_t {
    TargetGate Target;
    uint32_t ApplyFlags;
    Color_t Color;
    uint16_t Delay;
};

#define EffectMachinesCount 4

struct deviceConfig_t {
    DeviceStartMode StartUpMode;
    syncLedConfig_t SyncLeds;
    aSyncLedConfig_t AsyncLeds;
    rgbwLedConfig_t RgbStrip;
    i2cExpander_t I2cExpander;
    effectProcessor_t EffectMachines[EffectMachinesCount];
};
