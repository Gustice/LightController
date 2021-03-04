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
#include "DeviceTypes.h"

extern const char *wifiCfgFile;
extern const char *devCfgFile;
extern const char *mqttCfgFile;


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