#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"


static bool stringIsEqual(const char * expected, const char * actual);

TEST_CASE("Validating Test with linked cJSON", "[cJson]") 
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", 42);
    const char *sys_info = cJSON_PrintUnformatted(root);
    REQUIRE( stringIsEqual("{\"raw\":42}", sys_info) );
    free((void *)sys_info);
    cJSON_Delete(root);
}

static bool stringIsEqual(const char * expected, const char * actual)
{
    if(strcmp(expected, actual) == 0)
        return true;

    UNSCOPED_INFO("Expected " << expected);
    UNSCOPED_INFO("Actual " << actual);
    return false;
}







void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...)
{
    
}

esp_err_t xQueueSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait )
{
    return ESP_OK;
}

uint32_t esp_log_timestamp()
{
    return 0;
}

esp_err_t httpd_resp_set_type(httpd_req_t *r, const char *type)
{
    return ESP_OK;
}

