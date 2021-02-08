#pragma once

#include "ColorPosts.h"
#include <stdint.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t Init_WebFs(void);
typedef enum RgbChannel {
    None = -1,
    RgbiSync = 0,   // Only on Channel supported
    RgbwAsync,      // Only one Channel supported
    RgbwPwm,        // May be multiple channels
    I2cExpanderPwm, // May be multiple channels
};

typedef struct ReqColorIdx_def {
    RgbChannel channel;
    uint16_t chIdx;
    uint16_t portIdx;
} ReqColorIdx_t;

typedef struct ColorMsg_def {
    RgbChannel channel;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t intensity;
    uint8_t targetIdx;
} ColorMsg_t;

typedef struct GrayValMsg_def {
    RgbChannel channel;
    uint8_t gray[16];
    uint8_t intensity;
    uint8_t targetIdx;
} GrayValMsg_t;

typedef esp_err_t (*pChannelGetCallback)(ReqColorIdx_t , uint8_t *, size_t);

void SetupMyWeb(QueueHandle_t colorQ, QueueHandle_t grayQ, SemaphoreHandle_t newLedWebCmd,
    pChannelGetCallback getCbk);
