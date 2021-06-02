/**
 * @file ParamReader.c
 * @author Gustice
 * @brief Module to read/write files from/to SPIFFS
 * @details Derived from Espressif-Example "\examples\protocols\mqtt\tcp".
 *
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021
 */

#include "ParamReader.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>


const char *wifiCfgFile = "wifiConfigFile.bin";
const char *devCfgFile = "deviceConfigFile.bin";

static const char *TAG = "SpiFFs";
static esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs", // todo change to partition label
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true};

static bool active = false;

void Fs_SetupSpiFFs(void) {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    active = true;

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void unmountSpiFFs(void) {
    active = false;
    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}

static char *GetFileName(const char *file) {
    static char fileName[128];
    sprintf(fileName, "/spiffs/%s", file);
    return fileName;
}

esp_err_t Fs_SaveEntry(const char *file, void *stream, size_t length) {
    char *fName = GetFileName(file);
    if (!active) {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be saved", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen(fName, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s for writing", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Write stream to %s", fName);
    fwrite(stream, 1, length, f);
    fclose(f);
    return ESP_OK;
}

esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length) {
    char *fName = GetFileName(file);
    if (!active) {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be read", fName);
        return ESP_FAIL;
    }

    FILE *f = fopen(fName, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s for reading", fName);
        return ESP_FAIL;
    }

    // obtain file size:
    fseek(f, 0, SEEK_END);
    size_t lSize = ftell(f);
    rewind(f);

    if (lSize > length) {
        ESP_LOGE(TAG, "Provided buffer too small for file %s size", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Read stream from %s", fName);
    fread(stream, 1, length, f);
    ((uint8_t *)stream)[lSize] = 0; // terminate output
    fclose(f);
    return ESP_OK;
}

esp_err_t Fs_DeleteEntry(const char *file) {
    char *fName = GetFileName(file);
    if (!active) {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be deleted", fName);
        return ESP_FAIL;
    }

    struct stat st;
    if (stat(fName, &st) == 0) {
        // Delete it if it exists
        unlink(fName);
    }

    // Rename original file
    ESP_LOGI(TAG, "Removing file %s", fName);
    if (remove(fName) != 0) {
        ESP_LOGE(TAG, "Removing failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t Fs_CheckIfExists(const char *file) {
    if (!active)
        return ESP_FAIL;

    char *fName = GetFileName(file);
    struct stat st;
    if (stat(fName, &st) == 0) {
        return ESP_OK;
    }

    return ESP_FAIL;
}

#ifndef CONFIG_FS_ROOT
#define CONFIG_FS_ROOT "/spiffs/"
#endif

esp_err_t Fs_ReadFactoryConfiguration(factoryInfo_t *factorySet) {
    if (!active)
        return ESP_FAIL;

    char buf[512]; // Maximum 2 Pages // todo verify
    sprintf(buf, "%s%s", CONFIG_FS_ROOT, CONFIG_FACTORY_IMAGE_FILENAME);
    FILE *f = fopen(buf, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open Factory Configuration");
        return ESP_FAIL;
    }

    size_t rs = fread(buf, 1, sizeof(buf), f);

    ESP_LOGD(TAG, "ReadElements %d", rs);
    ESP_LOGD(TAG, "Read Text %s", buf);

    cJSON *root = cJSON_Parse(buf);
    const char *SerialNumber = cJSON_GetObjectItem(root, "SerialNumber")->valuestring;
    const char *HwVersion = cJSON_GetObjectItem(root, "HwVersion")->valuestring;
    const char *DeviceType = cJSON_GetObjectItem(root, "DeviceType")->valuestring;

    strcpy((char *)factorySet->SerialNumber, SerialNumber);
    strcpy((char *)factorySet->HwVersion, HwVersion);
    strcpy((char *)factorySet->DeviceType, DeviceType);

    fclose(f);
    cJSON_Delete(root);
    return ESP_OK;
}


esp_err_t ParseStripDefinition(cJSON *chRoot, const char *tockenName, stripConfig_t *config);


void IteratePhysicalChannels(deviceConfig_t *deviceSet, cJSON * channels){
    if (!cJSON_IsArray(channels)) {
        ESP_LOGW(TAG, "Error Parsing Outputs array");
        return;
    }

    int chIdx = -1;
    cJSON *ch;
    cJSON_ArrayForEach(ch, channels) {
        chIdx++;
        cJSON *chType = cJSON_GetObjectItem(ch, "Type");

        if (!cJSON_IsString(chType) || (chType->valuestring == NULL)) {
            ESP_LOGW(TAG, "Error Parsing Config-JSON of Channel %d: Cannot read Type", chIdx);
            continue;
        }

        cJSON *pDelay = cJSON_GetObjectItem(ch, "InitDelay");
        int delay = 0;
        if (cJSON_IsNumber(pDelay)) {
            delay = pDelay->valueint;
        }

        stripConfig_t *pStrip = nullptr;
        Color_t *pColor = nullptr;
        if (strcmp("SyncLedCh", chType->valuestring) == 0) {
            deviceSet->SyncLeds.IsActive = true;
            pStrip = &(deviceSet->SyncLeds.Strip);
            pColor = &(deviceSet->SyncLeds.Color);
            deviceSet->SyncLeds.Delay = (uint16_t)delay;
        } else if (strcmp("AsyncLedCh", chType->valuestring) == 0) {
            deviceSet->AsyncLeds.IsActive = true;
            pStrip = &(deviceSet->AsyncLeds.Strip);
            pColor = &(deviceSet->AsyncLeds.Color);
            deviceSet->AsyncLeds.Delay = (uint16_t)delay;
        } else if (strcmp("RgbStrip", chType->valuestring) == 0) {
            deviceSet->RgbStrip.IsActive = true;
            pStrip = &(deviceSet->RgbStrip.Strip);
            pColor = &(deviceSet->RgbStrip.Color);
            deviceSet->RgbStrip.ChannelCount = 1; // Preliminary Default
            deviceSet->RgbStrip.Delay = (uint16_t)delay;
        }
        if (pStrip != nullptr) {
            if (ParseStripDefinition(ch, "Strip", pStrip) != ESP_OK) {
                ESP_LOGW(TAG, "Error Parsing Config-JSON of channel %d: Cannot read Strip", chIdx);
                continue;
            }

            cJSON *color = cJSON_GetObjectItem(ch, "Color");
            if (!cJSON_IsArray(color) || cJSON_GetArraySize(color) != 4) {
                ESP_LOGW(TAG, "Error Parsing Config-JSON of Color in channel %d", chIdx);
                continue;
            }
            cJSON *pV;
            int pVIdx = 0;
            uint8_t cArr[4];

            cJSON_ArrayForEach(pV, color) { cArr[pVIdx++] = pV->valueint; }
            if (pColor != nullptr) {
                pColor->red = cArr[0];
                pColor->green = cArr[1];
                pColor->blue = cArr[2];
                pColor->white = cArr[3];
            }
        } else {
            if (strcmp("I2cExpander", chType->valuestring) == 0) {
                deviceSet->I2cExpander.IsActive = true;
                if (ParseStripDefinition(ch, "Device", &(deviceSet->I2cExpander.Device)) !=
                    ESP_OK) {
                    ESP_LOGW(
                        TAG, "Error Parsing Config-JSON of Channel %d: Cannot read Device", chIdx);
                    continue;
                }
            } else
                ESP_LOGE(
                    TAG, "Error Parsing Config-JSON: Type %s not recognized", chType->valuestring);
        }
    }
}

void IterateEffectChannels(deviceConfig_t *deviceSet, cJSON * effects){
    if (!cJSON_IsArray(effects)) {
        ESP_LOGW(TAG, "Error Parsing Outputs array");
        return;
    }

    int chIdx = -1;
    cJSON *ch;
    cJSON_ArrayForEach(ch, effects) {
        if (chIdx++ >= EffectMachinesCount)
            continue;
        cJSON *chType = cJSON_GetObjectItem(ch, "TargetGate");

        if (!cJSON_IsString(chType) || (chType->valuestring == NULL)) {
            ESP_LOGW(TAG, "Error Parsing Config-JSON of Channel %d: Cannot read Type", chIdx);
            continue;
        }
        effectProcessor_t *pProcessor = &(deviceSet->EffectMachines[chIdx]);

        if (strcmp("S", chType->valuestring) == 0)
            pProcessor->Target = TargetGate::SyncLed;
        else if (strcmp("A", chType->valuestring) == 0)
            pProcessor->Target = TargetGate::AsyncLed;
        else if (strcmp("R", chType->valuestring) == 0)
            pProcessor->Target = TargetGate::LedStrip;
        else if (strcmp("I", chType->valuestring) == 0)
            pProcessor->Target = TargetGate::I2cExpander;
        else {
            ESP_LOGE(TAG, "Error Parsing Config-JSON: Type %s not recognized", chType->valuestring);
            continue;
        }

        cJSON *pDelay = cJSON_GetObjectItem(ch, "InitDelay");
        int delay = 0;
        if (cJSON_IsNumber(pDelay)) {
            pProcessor->Delay = (uint16_t)pDelay->valueint;
        }

        cJSON *ports = cJSON_GetObjectItem(ch, "Ports");
        if (!cJSON_IsArray(ports) || cJSON_GetArraySize(ports) >=32) {
            ESP_LOGW(TAG, "Error Parsing Config-JSON of Ports in channel %d", chIdx);
            continue;
        }
        cJSON *pV;
        int pVIdx = 0;
        uint32_t applyFlags = 0;

        cJSON_ArrayForEach(pV, ports) { 
            int val =  pV->valueint;
            if ((val > 0) && (val <= 32)) {
                val = val-1;
                applyFlags |= ((uint32_t)1 << val);
            }
        }
        pProcessor->ApplyFlags = applyFlags;

        cJSON *color = cJSON_GetObjectItem(ch, "Color");
        if (!cJSON_IsArray(color) || cJSON_GetArraySize(color) != 4) {
            ESP_LOGW(TAG, "Error Parsing Config-JSON of Color in channel %d", chIdx);
            continue;
        }
        pVIdx = 0;
        uint8_t cArr[4];

        cJSON_ArrayForEach(pV, color) { cArr[pVIdx++] = pV->valueint; }
        pProcessor->Color.red = cArr[0];
        pProcessor->Color.green = cArr[1];
        pProcessor->Color.blue = cArr[2];
        pProcessor->Color.white = cArr[3];
    }
}

esp_err_t Fs_ReadDeviceConfiguration(deviceConfig_t *deviceSet) {
    if (!active)
        return ESP_FAIL;

    char buf[2048]; // Maximum 16 Pages // @todo verify
    sprintf(buf, "%s%s", CONFIG_FS_ROOT, CONFIG_DEVICE_CONFIGURATION_FILENAME);
    FILE *f = fopen(buf, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open Factory Configuration");
        return ESP_FAIL;
    }

    size_t rs = fread(buf, 1, sizeof(buf), f);
    ESP_LOGD(TAG, "ReadElements %d", rs);
    ESP_LOGD(TAG, "Read Text %s", buf);
    fclose(f);

    cJSON *root = cJSON_Parse(buf);

    cJSON *jsonConfig = cJSON_Parse(buf);
    if (jsonConfig == NULL) {
        ESP_LOGD(TAG, "Error Parsing Config-JSON");
        return ESP_FAIL;
    }

    cJSON *mode = cJSON_GetObjectItem(jsonConfig, "StartupMode");
    if (cJSON_IsString(mode) && (mode->valuestring != NULL)) {
        if (strcmp("RunDemo", mode->valuestring) == 0)
            deviceSet->StartUpMode = DeviceStartMode::RunDemo;
        else if (strcmp("StartImage", mode->valuestring) == 0)
            deviceSet->StartUpMode = DeviceStartMode::StartImage;
        else
            deviceSet->StartUpMode = DeviceStartMode::StartDark;
    }

    cJSON *channels = cJSON_GetObjectItem(jsonConfig, "Outputs");
    IteratePhysicalChannels(deviceSet, channels);

    cJSON *effects = cJSON_GetObjectItem(jsonConfig, "EffectProcessor");
    IterateEffectChannels(deviceSet, effects);

    cJSON_Delete(root);
    return ESP_OK;
}


esp_err_t ParseStripDefinition(cJSON *chRoot, const char *tockenName, stripConfig_t *config) {
    cJSON *strip = cJSON_GetObjectItem(chRoot, tockenName);
    if (strip == NULL)
        return ESP_FAIL;

    cJSON *count = cJSON_GetObjectItem(strip, "LedCount");
    if (!cJSON_IsNumber(count))
        return ESP_FAIL;

    config->LedCount = count->valueint;

    cJSON *channel = cJSON_GetObjectItem(strip, "Channel");
    if (!cJSON_IsString(channel) || (channel->valuestring == NULL))
        return ESP_FAIL;

    if (strcmp("RGBI", channel->valuestring) == 0)
        config->Channels = ColorChannels::RGBI;
    else if (strcmp("RGBW", channel->valuestring) == 0)
        config->Channels = ColorChannels::RGBW;
    else if (strcmp("RGB", channel->valuestring) == 0)
        config->Channels = ColorChannels::RGB;
    else if (strcmp("Gray", channel->valuestring) == 0)
        config->Channels = ColorChannels::Gray;
    else {
        config->Channels = ColorChannels::NoChannel;
        return ESP_FAIL;
    }

    return ESP_OK;
}
