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

extern const size_t FileBufferSize;
const size_t FileBufferSize = 4098;
char FileBuffer[FileBufferSize];
char FileName[32];
size_t FileSize = 0;


esp_err_t Fs_SaveEntry(const char *file, void *stream, size_t length) {
    memcpy(FileBuffer, stream, length);
    return ESP_OK;
}

esp_err_t Fs_ReadEntry(const char *file, void *stream, size_t length) {
    memcpy(stream, FileBuffer, FileSize);
}

esp_err_t Fs_CheckIfExists(const char *file) {
    return ESP_OK;
}

esp_err_t Fs_DeleteEntry(const char *file) {
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


void esp_restart()
{
    return;
}