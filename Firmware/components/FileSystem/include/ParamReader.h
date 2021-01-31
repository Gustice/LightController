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
extern "C"
{
#endif

#include <stdint.h>
#include <stdint.h>
#include "esp_err.h"

    extern const char *wifiCfgFile;
    extern const char *devCfgFile;
    extern const char *mqttCfgFile;

    typedef struct factoryInfo_def
    {
        char DeviceType[24];
        char SerialNumber[8]; // Something like SN:123ABC
        char HwVersion[32]; // V 1.00.00
        char SwVersion[12];
    } factoryInfo_t;

    typedef struct wifiConfig_def
    {
        uint8_t ssid[32];
        /** @note Length must be >= 8 (if not 0) */
        uint8_t password[64];
    } WifiConfig_t;

    void Fs_SetupSpiFFs(void);

    esp_err_t Fs_CheckIfExists(const char *file);
    esp_err_t Fs_SaveEntry(const char *file, void *stream, size_t length);
    esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length);
    esp_err_t Fs_DeleteEntry(const char *file);

    esp_err_t Fs_ReadFactoryConfiguration(factoryInfo_t * factorySet);

#ifdef __cplusplus
}
#endif