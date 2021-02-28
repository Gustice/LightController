/**
 * @file ParamReader.h
 * @author Gustice
 * @brief Module to read/write files from/to SPIFFS
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdint.h>
#include "Color.h"

extern const char *wifiCfgFile;
extern const char *devCfgFile;
extern const char *mqttCfgFile;

typedef struct factoryInfo_def { // @todo the container can be easily overwritten
    char DeviceType[24];
    char SerialNumber[12]; // Something like SN:123ABC
    char HwVersion[16];   // V 1.00.00
    char SwVersion[12];
} factoryInfo_t;

typedef struct wifiConfig_def {
    uint8_t ssid[32];
    /** @note Length must be >= 8 (if not 0) */
    uint8_t password[64];
    uint8_t max_connection;
} WifiConfig_t;

typedef enum ColorChannels {
    None = 0,
    RGBI,
    RGB,
    RGBW,
    Grey,
} ColorChannels_t;

typedef enum DeviceStartMode {
    RunDemo,
    StartDark,
    StartImage,
} DeviceStartMode_t;

typedef struct stripConfig_def {
    uint8_t LedCount;
    uint16_t Intensity;
    ColorChannels_t Channels;
} stripConfig_t;

typedef struct syncLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
} syncLedConfig_t;

typedef struct asyncLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
} aSyncLedConfig_t;

typedef struct rgbwLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
} rgbwLedConfig_t;

typedef struct i2cExpander_def {
    bool IsActive;
    stripConfig_t Device;
    uint8_t Address;
    uint16_t GrayValues[16];
} i2cExpander_t;

typedef struct deviceConfig_def {
    DeviceStartMode_t StartUpMode;
    syncLedConfig_t SyncLeds;
    aSyncLedConfig_t AsyncLeds;
    rgbwLedConfig_t RgbStrip;
    i2cExpander_t I2cExpander;
} deviceConfig_t;


void Fs_SetupSpiFFs(void);

esp_err_t Fs_CheckIfExists(const char *file);
esp_err_t Fs_SaveEntry(const char *file, void *stream, size_t length);
esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length);
esp_err_t Fs_DeleteEntry(const char *file);

esp_err_t Fs_ReadFactoryConfiguration(factoryInfo_t *factorySet);
esp_err_t Fs_ReadDeviceConfiguration(deviceConfig_t *deviceSet);

#ifdef __cplusplus
}
#endif