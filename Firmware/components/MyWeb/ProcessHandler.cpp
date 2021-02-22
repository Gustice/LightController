
#include "ProcessHandler.h"
#include "Color.h"
#include "MyWeb.h"
#include "cJSON.h"
#include <esp_log.h>
#include <sstream>
#include <string.h>
#include "WebUtils.h"

static const char *cModTag = "Web-ReqPcs";

#define CHECK_FILE_EXTENSION(filename, ext)                                                        \
    (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath) {
    const char *type = "text/plain";
    // Special case: File has no extension -> Route
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

void ParseJson(const char *buf, const char **labels, int *values, int length) {
    cJSON *root = cJSON_Parse(buf);
    for (size_t i = 0; i < length; i++) {
        values[i] = std::stoi(cJSON_GetObjectItem(root, labels[i])->valuestring);
    }
    cJSON_Delete(root);
}

static QueueHandle_t xColorQueue;
static QueueHandle_t xGrayQueue;
static esp_err_t (*GetChannelSettings)(ReqColorIdx_t channel, uint8_t * data, size_t length);

void SetQueueHandlesForPostH(QueueHandle_t colorQ, QueueHandle_t grayQ, pChannelGetCallback getCbk) {
    xColorQueue = colorQ;
    xGrayQueue = grayQ;
    GetChannelSettings = getCbk;
}

ApplyIndexes_t ParseApplyToString(char * applyTo) // @todo Exception handling badly needed :-/
{
    ApplyIndexes_t indexes;
    memset(&indexes, 0, sizeof(ApplyIndexes_t));

    if (applyTo == nullptr)
        return indexes;
    if (strlen(applyTo) == 0)
        return indexes;

    char * pC; // reference of Comma 
    char * pCn; // reference to next Comma 
    char * pD; // reference of Dot
    int ch, pos;
    pCn = strtok (applyTo,",");
    while (pCn != NULL)
    {
        pC = pCn;
        pCn = strtok (NULL,",");

        pD = strchr(pC,'.');
        if(pD != NULL)
        {
            ch = std::stoi(pC);
            pos = std::stoi(++pD);

            if ((ch <= 0) || (ch > ApplyToTargetChannels))
                continue;
            if ((pos <= 0) || (pos > ApplyToChannelWidth))
                continue;
            
            ch -= 1;
            pos -= 1;
        }
        else
        {
            int idx = std::stoi(pC);

            if ((idx <= 0) || (idx > ApplyToTargetChannels*ApplyToChannelWidth))
                continue;
            idx -= 1;
            ch = idx / ApplyToChannelWidth;
            pos = idx % ApplyToChannelWidth;
        }

        indexes.ApplyTo[ch] |= (1 << pos);
    }
    return indexes;
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
    const char *labels[size] = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11",
        "G12", "G13", "G14", "G15", "G16"};
    int values[size];
    ParseJson(message, labels, values, size);
    GrayValMsg_t msg;
    msg.channel = RgbChannel::I2cExpanderPwm;
    for (size_t i = 0; i < size; i++) {
        msg.gray[i] = (uint8_t)values[i];
    }

    msg.intensity = 0;
    msg.targetIdx = 0;

    ESP_LOGI(cModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d", msg.gray[0],
        msg.gray[1], msg.gray[2], msg.gray[3], msg.gray[4], msg.gray[5], msg.gray[6], msg.gray[7]);
    ESP_LOGI(cModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d", msg.gray[8],
        msg.gray[9], msg.gray[10], msg.gray[11], msg.gray[12], msg.gray[13], msg.gray[14],
        msg.gray[15]);

    if (xGrayQueue != nullptr) {
        if (xQueueSend(xGrayQueue, (void *)&msg, (TickType_t)10) != pdPASS)
            ESP_LOGE(cModTag, "Failed to send the Message");
    } else {
        ESP_LOGE(cModTag, "Queue-Handle not set");
    }

    return ESP_OK;
}

esp_err_t ProcessRgbiGet(char *message, const char **output) { 
    ReqColorIdx_t req {RgbChannel::RgbiSync, 0, 0};
    ColorMsg_t value;
    esp_err_t ret =  GetChannelSettings(req, (uint8_t *)&value, sizeof(ColorMsg_t));

    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "R", value.red);
    cJSON_AddNumberToObject(root, "G", value.green);
    cJSON_AddNumberToObject(root, "B", value.blue);
    cJSON_AddNumberToObject(root, "I", value.intensity);
    char *rendered = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    *output = rendered;
    return ESP_OK;
}

esp_err_t ProcessRgbwGet(char *message, const char **output) { return ESP_OK; }
esp_err_t ProcessRgbwSingleGet(char *message, const char **output) { return ESP_OK; }

esp_err_t ProcessGrayValuesGet(char *message, const char **output) { return ESP_OK; }


esp_err_t ProcessWiFiStatusSet(const char *message) {
    WifiConfig_t config;
    memset(&config, 0, sizeof(WifiConfig_t));

    cJSON *root = cJSON_Parse(message);
    const char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
    const char *password = cJSON_GetObjectItem(root, "password")->valuestring;
    strcpy((char *)config.ssid, ssid);
    strcpy((char *)config.password, password);
    cJSON_Delete(root);

    if (SaveWiFiConfig(&config) == ESP_OK)
        return ESP_OK;

    return ESP_FAIL;
}

esp_err_t ProcessWiFiStatusGet(char *message, const char **output) {
    if (*output != nullptr)
        return ESP_FAIL;

    char *buffer = new char[256];
    WifiConfig_t config; // Is actually ignored
    *output = "{\"wifiStatus\": \"WiFiParamNotSet\"}";
    if (LoadWiFiConfig(&config) == ESP_OK)
        *output = "{\"wifiStatus\": \"WiFiParamIsSet\"}";
    return ESP_OK;
}
esp_err_t ProcessResetWifiConfig(const char *messange) { return ResetWiFiConfig(); }


esp_err_t ProcessGetDeviceConfig(char *message, const char **output) {
    if (*output != nullptr)
        return ESP_FAIL;

    const size_t length = 2048;
    char *buffer = new char[length];
    *output = buffer;

    return Fs_ReadEntry("DeviceSetup.json", buffer, length);
}

esp_err_t ProcessSaveToPage(const char *message) {
    int page = std::stoi(message);
    return ESP_OK;
}

esp_err_t ProcessResetPages(const char *messange) { return ESP_OK; }
