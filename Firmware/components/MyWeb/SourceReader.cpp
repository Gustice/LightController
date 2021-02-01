#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "esp_vfs_semihost.h"
#include "lwip/apps/netbiosns.h"
// #include "mdns.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "sdmmc_cmd.h"

static bool active = false;
static const char *TAG = "WebSrcs";

const esp_vfs_spiffs_conf_t conf = {
    .base_path = CONFIG_WEB_MOUNT_POINT,
    .partition_label = "server",
    .max_files = 5,
    .format_if_mount_failed = false};

esp_err_t Init_WebFs(void) {
    ESP_LOGI(TAG, "Initializing SPIFFS-Web-Partition");
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    active = true;

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}


static const char *GetFileName(const char *file)
{
    static char fileName[128];
    sprintf(fileName, "%s/%s", CONFIG_WEB_MOUNT_POINT, file);
    return fileName;
}

esp_err_t WebFs_CheckIfExists(const char *file)
{
    if (!active)
        return ESP_FAIL;

    const char * fName = GetFileName(file);
    struct stat st;
    if (stat(fName, &st) == 0)
    {
        return ESP_OK;
    }

    return ESP_FAIL;
}

