#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "SoftAp.h"
#include "Utils.h"
#include <string.h>

static const char *TAG = "wifi";
const char *wifiCode = "wifi";

static void wifiAp_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                 void *event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(wifiApConfig_t *config) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &wifiAp_event_handler, NULL, NULL));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.ap.ssid, config->ssid, member_size(wifiApConfig_t, ssid));
    wifi_config.ap.ssid_len = (uint8_t)strlen((char *)config->ssid);
    memcpy(wifi_config.ap.password, config->password, member_size(wifiApConfig_t, password));
    wifi_config.ap.max_connection = config->max_connection;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    if (strlen((char *)wifi_config.ap.password) < 8)
        ESP_LOGE(TAG, "wifi password must be min 8 characters wide");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s", wifi_config.ap.ssid,
             wifi_config.ap.password);
}

static EventGroupHandle_t s_wifi_event_group;
const int MaxRetry = 3;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;

static void wifiSta_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                  void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MaxRetry) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(wifiApConfig_t *config) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiSta_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifiSta_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, config->ssid, member_size(wifiApConfig_t, ssid));
    memcpy(wifi_config.sta.password, config->password, member_size(wifiApConfig_t, password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed
     * for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which
     * event actually happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", config->ssid, config->password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", config->ssid, config->password);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_any_id));
    ESP_ERROR_CHECK(
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_got_ip));
    vEventGroupDelete(s_wifi_event_group);
}

void SetupSoftAccessPoint(wifiApConfig_t *config) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifiApConfig_t savedConfig;
    if (LoadWiFiConfig(&savedConfig) == ESP_OK) {
        ESP_LOGI(TAG, "WiFi-Config loaded, connecting to WiFi Access Point");
        wifi_init_sta(&savedConfig);
    } else {
        ESP_LOGI(TAG, "No WiFi-Config available, starting own WiFi Access Point");
        wifi_init_softap(config);
    }
}

esp_err_t SaveWiFiConfig(wifiApConfig_t *config) {
    nvs_handle_t nvsHandle;
    if (nvs_open("storage", NVS_READWRITE, &nvsHandle) != ESP_OK) { // Check init state
        ESP_LOGE(TAG, "Opening NVS failed");
        return ESP_FAIL;
    }
    esp_err_t err;
    err = nvs_set_blob(nvsHandle, wifiCode, config, sizeof(wifiApConfig_t));
    err = nvs_commit(nvsHandle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saving of wifi parameter successful");
    } else {
        ESP_LOGE(TAG, "Saving of wifi parameter failed");
        return ESP_FAIL;
    }
    // Close
    nvs_close(nvsHandle);

    return ESP_OK;
}

esp_err_t ResetWiFiConfig(void) {
    nvs_handle_t nvsHandle;
    if (nvs_open("storage", NVS_READWRITE, &nvsHandle) != ESP_OK) { // Check init state
        ESP_LOGE(TAG, "Opening NVS failed");
        return ESP_FAIL;
    }
    esp_err_t err;
    err = nvs_erase_key(nvsHandle, wifiCode);
    err = nvs_commit(nvsHandle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Erasing of wifi parameter successful");
    } else {
        ESP_LOGE(TAG, "Erasing of wifi parameter failed");
        return ESP_FAIL;
    }
    // Close
    nvs_close(nvsHandle);

    return ESP_OK;
}

esp_err_t LoadWiFiConfig(wifiApConfig_t *config) {
    nvs_handle_t nvsHandle;
    if (nvs_open("storage", NVS_READONLY, &nvsHandle) != ESP_OK) { // Check init state
        ESP_LOGE(TAG, "Opening NVS failed");
        return ESP_FAIL;
    }
    esp_err_t err;
    size_t size = sizeof(wifiApConfig_t);
    err = nvs_get_blob(nvsHandle, wifiCode, config, &size);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGD(TAG, "Loading of wifi parameter failed, not initialized yet");
        return ESP_FAIL;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Loading of wifi parameter failed");
        return ESP_FAIL;
    }
    // Close
    nvs_close(nvsHandle);

    return ESP_OK;
}