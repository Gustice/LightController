#pragma once


#include <stdint.h>
#include "ColorPosts.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void SetupMyWeb(QueueHandle_t colorQ, QueueHandle_t grayQ, SemaphoreHandle_t ledDataSignal);

esp_err_t Init_WebFs(void);
typedef enum RgbChannel {
    None = -1,
    RgbiSync = 0,
    RgbwAsync,
    RgbwPwm,
    I2cExpanderPwm,
};

typedef struct ColorMsg_def {
    RgbChannel channel;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t intensity;
    uint8_t targetIdx;
}ColorMsg_t;

typedef struct GrayValMsg_def {
    RgbChannel channel;
    uint8_t gray[16];
    uint8_t intensity;
    uint8_t targetIdx;
}GrayValMsg_t;

