#pragma once

#include <stdint.h>

typedef enum RgbChannel {
    None = -1,
    RgbiSync = 0,   // Only on Channel supported
    RgbwAsync,      // Only one Channel supported
    RgbwPwm,        // May be multiple channels
    I2cExpanderPwm, // May be multiple channels
} eRgbChannel_t;

typedef struct ReqColorIdx_def {
    RgbChannel type; 
    uint16_t chIdx; 
    uint16_t portIdx;
} ReqColorIdx_t;

#define ApplyToTargetChannels 5
#define ApplyToChannelWidth 32
typedef struct ApplyIndexes_def {
    uint16_t Items;
    uint16_t Errors;
    ReqColorIdx_t FirstTarget;
    uint32_t ApplyTo[ApplyToTargetChannels];
} ApplyIndexes_t;

typedef struct ColorMsg_def {
    RgbChannel channel;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t intensity;
    ApplyIndexes_t apply;
} ColorMsg_t;

typedef struct GrayValMsg_def {
    RgbChannel channel;
    uint8_t gray[16];
    uint8_t intensity;
    uint8_t targetIdx;
    ApplyIndexes_t apply;
} GrayValMsg_t;