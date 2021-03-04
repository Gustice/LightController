#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_err.h"

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
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

uint32_t esp_log_timestamp()
{
    return 0;
}

