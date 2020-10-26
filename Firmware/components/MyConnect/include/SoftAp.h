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

/**
 * @brief WiFi-Configuration
 * @note This is a subset of wifi_ap_config_t parameters
 */
typedef struct wifiApConfig_def
{
    uint8_t ssid[32];
    /** @note Length >= 8 */
    uint8_t password[64];
    uint8_t max_connection;
} wifiApConfig_t;

/**
 * @brief Setup Access Point
 * 
 * @param config WiFi-Configuration
 */
void SetupSoftAccessPoint(wifiApConfig_t * config);


esp_err_t SaveWiFiConfig(wifiApConfig_t *config);
esp_err_t ResetWiFiConfig(void);
esp_err_t LoadWiFiConfig(wifiApConfig_t *config);


#ifdef __cplusplus
}
#endif
