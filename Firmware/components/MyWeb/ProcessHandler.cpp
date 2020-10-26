
#include "ProcessHandler.h"
#include "Color.h"
#include "MyWeb.h"
#include "cJSON.h"
#include <esp_log.h>
#include <sstream>

static const char *cModTag = "Web-ReqProcessor";

void ParseJson(const char *buf, const char **labels, int *values, int length) {
    cJSON *root = cJSON_Parse(buf);
    for (size_t i = 0; i < length; i++) {
        values[i] = std::stoi(cJSON_GetObjectItem(root, labels[i])->valuestring);
    }
    cJSON_Delete(root);
}

static QueueHandle_t xColorQueue;
static QueueHandle_t xGrayQueue;

void SetQueueHandlesForPostH(QueueHandle_t colorQ, QueueHandle_t grayQ) {
    xColorQueue = colorQ;
    xGrayQueue = grayQ;
}

ColorMsg_t SetColorObj(int r, int g, int b, int w, int i) {
    ColorMsg_t ret{
        .channel = RgbChannel::None,
        .red = (uint8_t)r,
        .green = (uint8_t)g,
        .blue = (uint8_t)b,
        .white = (uint8_t)w,
        .intensity = (uint8_t)i,
        .targetIdx = 0,
    };
    return ret;
}

void SendColorQueue(ColorMsg_t msg) {
    ESP_LOGI(cModTag, "Light control: red = %d, green = %d, blue = %d, white = %d, intensity = %d",
             msg.red, msg.green, msg.blue, msg.white, msg.intensity);

    if (xColorQueue != nullptr) {
        if (xQueueSend(xColorQueue, (void *)&msg, (TickType_t)10) != pdPASS)
            ESP_LOGE(cModTag, "Failed to send the Message");
    } else {
        ESP_LOGE(cModTag, "Queue-Handle not set");
    }
}

esp_err_t ProcessRgbiPost(const char *message) {
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    int values[size];
    ParseJson(message, labels, values, size);

    cJSON *root = cJSON_Parse(message);
    const char *apply = cJSON_GetObjectItem(root, "applyTo")->valuestring;
    cJSON_Delete(root);

    ColorMsg_t msg = SetColorObj(values[0], values[1], values[2], 0, values[3]);

    msg.targetIdx = 0; // All
    msg.channel = RgbChannel::RgbiSync;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessRgbwPost(const char *message) {
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    int values[size];
    ParseJson(message, labels, values, size);

    cJSON *root = cJSON_Parse(message);
    const char *apply = cJSON_GetObjectItem(root, "applyTo")->valuestring;
    cJSON_Delete(root);

    ColorMsg_t msg = SetColorObj(values[0], values[1], values[2], values[3], 0);
    msg.channel = RgbChannel::RgbwAsync;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessRgbwSinglePost(const char *message) {
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    int values[size];
    ParseJson(message, labels, values, size);

    ColorMsg_t msg = SetColorObj(values[0], values[1], values[2], values[3], 0);
    msg.channel = RgbChannel::RgbwPwm;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessGrayValuesPost(const char *message) {
    const int size = 16;
    const char *labels[size] = {"G1", "G2",  "G3",  "G4",  "G5",  "G6",  "G7",  "G8",
                                "G9", "G10", "G11", "G12", "G13", "G14", "G15", "G16"};
    int values[size];
    ParseJson(message, labels, values, size);
    GrayValMsg_t msg;
    msg.channel = RgbChannel::I2cExpanderPwm;
    for (size_t i = 0; i < size; i++) {
        msg.gray[i] = (uint8_t)values[i];
    }

    msg.intensity = 0;
    msg.targetIdx = 0;

    ESP_LOGI(cModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d", 
        msg.gray[0], msg.gray[1], msg.gray[2], msg.gray[3], msg.gray[4], msg.gray[5], msg.gray[6], msg.gray[7]);
    ESP_LOGI(cModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d", 
        msg.gray[8], msg.gray[9], msg.gray[10], msg.gray[11], msg.gray[12], msg.gray[13], msg.gray[14], msg.gray[15]);

    if (xGrayQueue != nullptr) {
        if (xQueueSend(xGrayQueue, (void *)&msg, (TickType_t)10) != pdPASS)
            ESP_LOGE(cModTag, "Failed to send the Message");
    } else {
        ESP_LOGE(cModTag, "Queue-Handle not set");
    }


    return ESP_OK;
}

esp_err_t ProcessRgbiGet(char *message) { return ESP_OK; }

esp_err_t ProcessRgbwGet(char *message) { return ESP_OK; }

esp_err_t ProcessRgbwSingleGet(char *message) { return ESP_OK; }

esp_err_t ProcessGrayValuesGet(char *message) { return ESP_OK; }