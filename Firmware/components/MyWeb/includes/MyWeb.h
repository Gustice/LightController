/**
 * @file MyWeb.h
 * @author Gustice
 * @brief Web-API-Includes
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 */
#pragma once

#include <stdint.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ApiDataTypes.h"
#include "DeviceTypes.h"

esp_err_t Init_WebFs(void);

typedef esp_err_t (*pChannelGetCallback)(ReqColorIdx_t , uint8_t *, size_t);

void SetupMyWeb(QueueHandle_t colorQ, QueueHandle_t grayQ, SemaphoreHandle_t newLedWebCmd,
    pChannelGetCallback getCbk, deviceConfig_t * stationConfig);
