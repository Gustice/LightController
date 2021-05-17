#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "cJSON.h"
#include "catch.hpp"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static bool stringIsEqual(const char *expected, const char *actual);

TEST_CASE("Validating Test with linked cJSON", "[cJson]") {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", 42);
    const char *sys_info = cJSON_PrintUnformatted(root);
    REQUIRE(stringIsEqual("{\"raw\":42}", sys_info));
    free((void *)sys_info);
    cJSON_Delete(root);
}


struct FileInfo_t {
    const char *Name;
    const size_t Size;
};

FileInfo_t FsFiles[] =
    {
        {"Test.json", 1234},
        {"Config.json", 8971},
        {nullptr, 0},
};

// Aimed Format is : { "files": [{"file": "Test.json", "size": 1234} ,{"file": "Config.json",
// "size": 8971} ]}
TEST_CASE("ArrayOutput with cJSON", "[cJson]") {
    char *string = NULL;
    cJSON *files = NULL;
    cJSON *file = NULL;
    size_t index = 0;

    cJSON *jsonObject = cJSON_CreateObject();

    files = cJSON_CreateArray();
    if (files == NULL) {
        cJSON_Delete(jsonObject);
        // return string;
    }

    cJSON_AddItemToObject(jsonObject, "files", files);

    int idx = 0;
    while (FsFiles[idx].Name != nullptr) {
        file = cJSON_CreateObject();
        if (file == NULL) {
            if (files == NULL) {
                cJSON_Delete(jsonObject);
                // return string;
            }
        }
        cJSON_AddItemToArray(files, file);
        cJSON_AddStringToObject(file, "file", FsFiles[idx].Name);
        cJSON_AddNumberToObject(file, "size", FsFiles[idx].Size);
        idx++;
    }

    string = cJSON_Print(jsonObject);
    if (string == NULL) {
        fprintf(stderr, "Failed to print jsonObject.\n");
    }
    printf("%s", string);
    REQUIRE( true);

    cJSON_Delete(jsonObject);
}


static bool stringIsEqual(const char *expected, const char *actual) {
    if (strcmp(expected, actual) == 0)
        return true;

    UNSCOPED_INFO("Expected " << expected);
    UNSCOPED_INFO("Actual " << actual);
    return false;
}


void esp_log_write(esp_log_level_t level, const char *tag, const char *format, ...) {}

uint32_t esp_log_timestamp() { return 0; }

esp_err_t httpd_resp_set_type(httpd_req_t *r, const char *type) { return ESP_OK; }
