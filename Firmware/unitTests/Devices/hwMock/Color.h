#pragma once

#include <stdint.h>

/**
 * @brief Color Structure
 * @details Defines red/green/blue and white color channel values
 */
typedef struct Color_def {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
} Color_t;