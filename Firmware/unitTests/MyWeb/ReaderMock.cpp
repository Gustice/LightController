#include "TestUtils.h"
#include "ParamReader.h"
#include "ProcessHandler.h"

esp_err_t SaveWiFiConfig(WifiConfig_t *config) 
{
    return ESP_OK;
}
esp_err_t LoadWiFiConfig(WifiConfig_t *config)
{
    return ESP_OK;
}

esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length)
{
    return ESP_OK;
}

esp_err_t ResetWiFiConfig(void)
{
    return ESP_OK;
}

ColorMsg_t LastColorMsg;
GrayValMsg_t LastGrayValMsg;

esp_err_t xQueueSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait )
{
    if (xQueue == (QueueHandle_t)1)
    {
        memcpy(&LastColorMsg, pvItemToQueue, sizeof(ColorMsg_t));
        return ESP_OK;
    }
    if (xQueue == (QueueHandle_t)2)
    {
        memcpy(&LastGrayValMsg, pvItemToQueue, sizeof(GrayValMsg_t));
        return ESP_OK;
    }
    return ESP_FAIL;
}

