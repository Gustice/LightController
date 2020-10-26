#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "espMock/esp_log.h"
#include "espMock/esp_err.h"
#include "espMock/freertos/FreeRTOS.h"

TEST_CASE("Validating Testrunner", "") 
{
    REQUIRE( true );
}


uint32_t esp_log_timestamp(void)
{
    return 0;
}

void esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    UNSCOPED_INFO("Log-Message : " << level << " : " << tag );
    // va_list list;
    // va_start(list, format);
    // esp_log_writev(level, tag, format, list);
    // va_end(list);
}

const char *esp_err_to_name(esp_err_t code)
{
    return "Error";
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    // esp_error_check_failed_print("ESP_ERROR_CHECK", rc, file, line, function, expression);
    // abort();
}

// TickType_t pdMS_TO_TICKS(uint32_t ms) {  return ms;  }

void myassert(const char *, int, const char *) {}
void myassert_func(const char *, int, const char *, const char *) {}

int printfTaggedLine(esp_log_level_t ll, char const *tag, char const *format,
                     ...) { // This takes VarArgs
    int retVal;
    va_list args;
    va_start(args, format);
    printf("  %s: ", tag);
    retVal = vprintf(format, args);
    printf("\n");
    va_end(args);
    return (retVal);
}

int printfLine(char const *format, ...) { // This takes VarArgs
    int retVal;
    va_list args;
    va_start(args, format);
    retVal = vprintf(format, args);
    printf("\n");
    va_end(args);
    return (retVal);
}