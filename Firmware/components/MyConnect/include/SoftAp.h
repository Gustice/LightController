/**
 * @file SoftAp.h
 * @author Gustice
 * @brief Software Access-Point
 * @version 0.1
 * @date 2020-10-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "ParamReader.h"

/**
 * @brief Setup Access Point
 * 
 * @param config WiFi-Configuration
 */
void SetupSoftAccessPoint(WifiConfig_t * config);

esp_err_t SaveWiFiConfig(WifiConfig_t *config);
esp_err_t ResetWiFiConfig(void);
esp_err_t LoadWiFiConfig(WifiConfig_t *config);


#ifdef __cplusplus
}
#endif
