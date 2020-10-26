/**
 * @file iGpioInclude.h
 * @author Gustice
 * @brief Private GPIO-Include file
 * @details Untangles user implementation from driver, for test purpose
 * @version 0.1
 * @date 2020-10-08
 * 
 * @copyright Copyright (c) 2020
 */

#pragma once

#include "GpioPort.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_intr_alloc.h"