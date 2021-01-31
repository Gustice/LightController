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
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "cJSON.h"


const char *wifiCfgFile = "wifiConfigFile.bin";
const char *devCfgFile = "deviceConfigFile.bin";

static const char *TAG = "SpiFFs";
static esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
    };

static bool active = false;

void Fs_SetupSpiFFs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    active = true;

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void unmountSpiFFs(void)
{
    active = false;
    // All done, unmount partition and disable SPIFFS
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}

char *GetFileName(char *file)
{
    static char fileName[128];
    sprintf(fileName, "/spiffs/%s", file);
    return fileName;
}

esp_err_t Fs_SaveEntry(const char *file, void *stream, size_t length)
{
    char *fName = GetFileName(file);
    if (!active)
    {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be saved", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen(fName, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file %s for writing", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Write stream to %s", fName);
    fwrite(stream, 1, length, f);
    fclose(f);
    return ESP_OK;
}

esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length)
{
    char *fName = GetFileName(file);
    if (!active)
    {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be read", fName);
        return ESP_FAIL;
    }

    FILE *f = fopen(fName, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file %s for Read", fName);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Read stream from %s", fName);
    fread(stream, 1, length, f);
    fclose(f);
    return ESP_OK;
}

esp_err_t Fs_DeleteEntry(const char *file)
{
    char *fName = GetFileName(file);
    if (!active)
    {
        ESP_LOGE(TAG, "Setup not finished. File '%s' cannot be deleted", fName);
        return ESP_FAIL;
    }

    struct stat st;
    if (stat(fName, &st) == 0)
    {
        // Delete it if it exists
        unlink(fName);
    }

    // Rename original file
    ESP_LOGI(TAG, "Removing file %s", fName);
    if (remove(fName) != 0)
    {
        ESP_LOGE(TAG, "Removing failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t Fs_CheckIfExists(const char *file)
{
    if (!active)
        return ESP_FAIL;

    char *fName = GetFileName(file);
    struct stat st;
    if (stat(fName, &st) == 0)
    {
        return ESP_OK;
    }

    return ESP_FAIL;
}

esp_err_t Fs_ReadFactoryConfiguration(factoryInfo_t *factorySet)
{
    if (!active)
        return ESP_FAIL;

    char buf[512]; // Maximum 2 Pages // todo verify
    sprintf(buf, "/spiffs/%s", CONFIG_DEVICE_CONFIGURATION_FILENAME);
    FILE *f = fopen(buf, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open Factory Configuration");
        return ESP_FAIL;
    }

    memset(buf, 0, sizeof(buf));
    size_t rs = fread(buf, 1, sizeof(buf), f);

    ESP_LOGD(TAG, "ReadElements %d", rs);
    ESP_LOGD(TAG, "Read Text %s", buf);

    cJSON *root = cJSON_Parse(buf);
    const char *SerialNumber = cJSON_GetObjectItem(root, "SerialNumber")->valuestring;
    const char *HwVersion = cJSON_GetObjectItem(root, "HwVersion")->valuestring;
    const char *DeviceType = cJSON_GetObjectItem(root, "DeviceType")->valuestring;
    fclose(f);

    strcpy((char *)factorySet->SerialNumber, SerialNumber);
    strcpy((char *)factorySet->HwVersion, HwVersion);
    strcpy((char *)factorySet->DeviceType, DeviceType);
    return ESP_OK;
}
