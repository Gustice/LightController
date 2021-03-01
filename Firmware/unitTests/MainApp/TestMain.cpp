#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "esp_log.h"
#include "esp_err.h"
#include "Color.h"

TEST_CASE("Validating Toolchain", "[toolchain]") 
{
    REQUIRE(true);
}

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...)
{
    
}

uint32_t esp_log_timestamp()
{
    return 0;
}

const Color_t color_Black       = {0x00, 0x00, 0x00, 0x00}; ///< Black color structure constant
const Color_t color_Red         = {0xFF, 0x00, 0x00, 0x00}; ///< Red color structure constant
const Color_t color_Green       = {0x00, 0xFF, 0x00, 0x00}; ///< Green color structure constant
const Color_t color_Blue        = {0x00, 0x00, 0xFF, 0x00}; ///< Blue color structure constant
const Color_t color_Magenta     = {0x80, 0x00, 0x80, 0x00}; ///< Magenta color structure constant
const Color_t color_Cyan        = {0x00, 0x80, 0x80, 0x00}; ///< Cyan color structure constant
const Color_t color_Yellow      = {0x80, 0x80, 0x00, 0x00}; ///< Yellow color structure constant
const Color_t color_White       = {0x55, 0x55, 0x55, 0x00}; ///< White color structure constant
const Color_t color_WarmWhite   = {0x7F, 0x4F, 0x4F, 0x00}; ///< Warm white color structure constant (with higher red channel value)
const Color_t color_ColdWhite   = {0x4F, 0x4F, 0x7F, 0x00}; ///< Black color structure constant (with higher blue channel value)
const Color_t color_SpookyWhite = {0x4F, 0x7F, 0x4F, 0x00}; ///< Black color structure constant (with higher green channel value)

