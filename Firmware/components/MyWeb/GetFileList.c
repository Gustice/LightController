#include "cJSON.h"
#include "esp_netif.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include <esp_event.h>
#include <esp_http_server.h>
#include <stdio.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "sys/dirent.h"
#include "esp_spiffs.h"

const char * TAG = "FileScan";

char* GetFileList(void)
{
    // char entrysize[16];
    // entrysize[0] = '\0';
    // const char * dirpath = "/";

    // cJSON *files = NULL, *file = NULL;
    // cJSON *jsonObject = cJSON_CreateObject();

    // struct dirent *entry;
    // DIR *dir = opendir(dirpath);
    // if (!dir) {
    //     return NULL;
    // }
    
    // files = cJSON_CreateArray();
    // if (files != NULL) {
        
    //     cJSON_AddItemToObject(jsonObject, "files", files);

    //     while ((entry = readdir(dir)) != NULL) {
    //         // strlcpy(entrypath + dirpath_len, entry->d_name, sizeof(entrypath) - dirpath_len);
    //         // if (stat(entrypath, &entry_stat) == -1) {
    //         //     ESP_LOGE(TAG, "Failed to stat: %s", entry->d_name);
    //         //     continue;
    //         // }

    //         if (entry->d_type != DT_DIR)
    //         {
    //             ESP_LOGI(TAG, "Found %s (%s bytes)", entry->d_name, entrysize);

    //             file = cJSON_CreateObject();
    //             if (file == NULL) break;
    //             cJSON_AddItemToArray(files, file);
    //             cJSON_AddStringToObject(file, "file", entry->d_name);
    //             cJSON_AddStringToObject(file, "size", entrysize);
    //         }
    //     }
    // }

    // char * output = cJSON_PrintUnformatted(jsonObject);
    // cJSON_Delete(jsonObject);
    // closedir(dir);

    // return output;
    return NULL;
}

