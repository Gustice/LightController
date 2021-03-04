/**
 * @file ProcessHandler.cpp
 * @author Gustice
 * @brief Implementation fo Reqeuest-Handlers
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 */

#include "ProcessHandler.h"
#include "Color.h"
#include "MyWeb.h"
#include "WebUtils.h"
#include "cJSON.h"
#include <esp_log.h>
#include <sstream>
#include <string.h>

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

static QueueHandle_t xColorQueue;
static QueueHandle_t xGrayQueue;
static esp_err_t (*GetChannelSettings)(ReqColorIdx_t channel, uint8_t *data, size_t length);
static deviceConfig_t * pStationConfig;

void SetQueueHandlesForPostH(
    QueueHandle_t colorQ, QueueHandle_t grayQ, pChannelGetCallback getCbk, deviceConfig_t * stationConfig) {
    xColorQueue = colorQ;
    xGrayQueue = grayQ;
    GetChannelSettings = getCbk;
    pStationConfig = stationConfig;
}

ReqColorIdx_t ParseApplyToElement(char *element) {
    ReqColorIdx_t idx{
        .type = RgbChannel::None_Error,
        .chIdx = 0,
        .portIdx = 0,
    };

    char *pD; // reference of Dot
    int ch, pos;

    pD = strchr(element, '.');
    if (pD != NULL) {
        ch = std::stoi(element);
        pos = std::stoi(++pD);

        if ((ch <= 0) || (ch > ApplyToTargetChannels))
            return idx;
        if ((pos <= 0) || (pos > ApplyToChannelWidth))
            return idx;

        ch -= 1;
        pos -= 1;
    } else {
        int i = std::stoi(element);

        if (i <= 0)
            return idx;
        if (i > ApplyToTargetChannels * ApplyToChannelWidth)
            return idx;

        i -= 1;
        ch = i / ApplyToChannelWidth;
        pos = i % ApplyToChannelWidth;
    }

    idx.type = RgbChannel::Undefined;
    idx.chIdx = ch;
    idx.portIdx = pos;
    return idx;
}

/**
 * @brief Parse Apply-String in complex mode
 * @details Tries to parse comma seperated numbers in format: "1,2,3" or "1.1,1.2"
 *      in arbitrary order or format,
 *  Spans are also possible 'x' applies to all, 1-3 applies to 1 to 3 inclusive
 *
 * @param applyTo list of indexes to parse and translate to index-variable
 * @param indexes container to place found indexes in
 */
void ParseApplyToString(
    char *applyTo, ApplyIndexes_t *indexes) // @todo Exception handling badly needed :-/
{

    if (applyTo == nullptr || indexes == nullptr) {
        indexes->Errors++;
        return;
    }

    size_t len = strlen(applyTo);
    if (len == 0)
        return;

    if (len == 1) {
        if ((applyTo[0] == 'x') || (applyTo[0] == 'X')) {
            for (size_t i = 0; i < ApplyToTargetChannels; i++) {
                indexes->ApplyTo[i] = 0xFFFFFFFF;
                indexes->Items += 32;
            }
            return;
        }
    }

    bool first = true;
    char *pC;  // reference of Comma
    char *pCn; // reference to next Comma
    char *pR;  // reference to Range sign
    ReqColorIdx_t idx{
        .type = RgbChannel::None_Error,
        .chIdx = 0,
        .portIdx = 0,
    };

    pCn = strtok(applyTo, ",");
    while (pCn != NULL) {
        pC = pCn;
        pCn = strtok(NULL, ",");

        pR = strchr(pC, '-');
        if (pR != NULL) {
            char *pPre = pR - 1;
            while (pPre >= pC && (*pPre < '0') && (*pPre > '9')) {
                pPre--;
            }

            if (pPre < pC) // On limit no number between delimiter and minus
            {
                indexes->Errors++;
                continue;
            }

            idx = ParseApplyToElement(pC);
            if (idx.type == RgbChannel::None_Error) {
                indexes->Errors++;
                continue;
            }

            ReqColorIdx_t toIdx = ParseApplyToElement(pR + 1);
            if (toIdx.type == RgbChannel::None_Error) {
                indexes->Errors++;
                continue;
            }

            size_t from = ApplyToChannelWidth * idx.chIdx + idx.portIdx;
            size_t to = ApplyToChannelWidth * toIdx.chIdx + toIdx.portIdx;

            if (from >= to) {
                indexes->Errors++;
                continue;
            }


            for (; from <= to; from++) {
                int ch = from / ApplyToChannelWidth;
                int pos = from % ApplyToChannelWidth;
                indexes->ApplyTo[ch] |= (1 << pos);
                indexes->Items++;
            }
        } else {
            idx = ParseApplyToElement(pC);
            if (idx.type == RgbChannel::None_Error) {
                indexes->Errors++;
                continue;
            }

            indexes->ApplyTo[idx.chIdx] |= (1 << idx.portIdx);
            indexes->Items++;
        }

        if (first) {
            first = false;
            indexes->FirstTarget.chIdx = idx.chIdx;
            indexes->FirstTarget.portIdx = idx.portIdx;
        }
    }
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

esp_err_t ProcessRgbiPost(const char *message, const char **output) {
    ColorMsg_t msg;
    memset(&msg, 0, sizeof(ColorMsg_t));
    msg.channel = RgbChannel::RgbiSync;

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    uint8_t *values[size] = {&(msg.red), &(msg.green), &(msg.blue), &(msg.intensity)};

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < size; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }
    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    ParseApplyToString(apply, &msg.apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.apply.Items == 0 && msg.apply.Errors == 0)
        msg.apply.ApplyTo[0] = 1;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessRgbwPost(const char *message, const char **output) {
    ColorMsg_t msg;
    memset(&msg, 0, sizeof(ColorMsg_t));
    msg.channel = RgbChannel::RgbwAsync;

    int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(msg.red), &(msg.green), &(msg.blue), &(msg.white)};
    
    int maxStep = size;
    if (pStationConfig->AsyncLeds.Strip.Channels != ColorChannels::RGBW)
        maxStep = 3;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }

    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    ParseApplyToString(apply, &msg.apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.apply.Items == 0 && msg.apply.Errors == 0)
        msg.apply.ApplyTo[0] = 1;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessRgbwSinglePost(const char *message, const char **output) {
    ColorMsg_t msg;
    memset(&msg, 0, sizeof(ColorMsg_t));
    msg.channel = RgbChannel::RgbwPwm;

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(msg.red), &(msg.green), &(msg.blue), &(msg.white)};

    int maxStep = size;
    if (pStationConfig->RgbStrip.Strip.Channels != ColorChannels::RGBW)
        maxStep = 3;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }

    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    ParseApplyToString(apply, &msg.apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.apply.Items == 0 && msg.apply.Errors == 0)
        msg.apply.ApplyTo[0] = 1;

    SendColorQueue(msg);
    return ESP_OK;
}

esp_err_t ProcessGrayValuesPost(const char *message, const char **output) {
    GrayValMsg_t msg;
    const int size = 16;
    const char *labels[size] = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11",
        "G12", "G13", "G14", "G15", "G16"};
    int values[size];

    int maxStep = pStationConfig->I2cExpander.Device.LedCount;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        msg.gray[i]  = (uint8_t)e->valueint;
    }
    // Not needed here
    // char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    // ParseApplyToString(apply, &msg.apply);
    msg.channel = RgbChannel::I2cExpanderPwm;
    msg.intensity = 0;
    msg.targetIdx = 0;
    cJSON_Delete(root);

    ESP_LOGD(cModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d", msg.gray[0],
        msg.gray[1], msg.gray[2], msg.gray[3], msg.gray[4], msg.gray[5], msg.gray[6], msg.gray[7]);
    ESP_LOGD(cModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d", msg.gray[8],
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

esp_err_t ProcessRgbiGet(const char *message, const char **output) {
    ReqColorIdx_t req{RgbChannel::RgbiSync, 0, 0};
    ColorMsg_t value;
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);

    req.chIdx = idx.FirstTarget.chIdx;
    req.portIdx = idx.FirstTarget.portIdx;
    esp_err_t ret = GetChannelSettings(req, (uint8_t *)&value, sizeof(ColorMsg_t));

    cJSON * root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    uint8_t *values[size] = {&(value.red), &(value.green), &(value.blue), &(value.intensity)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessRgbwGet(const char *message, const char **output) {
    ReqColorIdx_t req{RgbChannel::RgbwAsync, 0, 0};
    ColorMsg_t value;
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);

    req.chIdx = idx.FirstTarget.chIdx;
    req.portIdx = idx.FirstTarget.portIdx;
    esp_err_t ret = GetChannelSettings(req, (uint8_t *)&value, sizeof(ColorMsg_t));

    cJSON * root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    uint8_t *values[size] = {&(value.red), &(value.green), &(value.blue), &(value.white)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessRgbwSingleGet(const char *message, const char **output) {
    ReqColorIdx_t req{RgbChannel::RgbwPwm, 0, 0};
    ColorMsg_t value;
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);

    req.chIdx = idx.FirstTarget.chIdx;
    req.portIdx = idx.FirstTarget.portIdx;
    esp_err_t ret = GetChannelSettings(req, (uint8_t *)&value, sizeof(ColorMsg_t));

    cJSON * root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    uint8_t *values[size] = {&(value.red), &(value.green), &(value.blue), &(value.white)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessGrayValuesGet(const char *message, const char **output) {
    ReqColorIdx_t req{RgbChannel::I2cExpanderPwm, 0, 0};
    GrayValMsg_t value;
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);

    req.chIdx = idx.FirstTarget.chIdx;
    req.portIdx = idx.FirstTarget.portIdx;
    esp_err_t ret = GetChannelSettings(req, (uint8_t *)&value, sizeof(GrayValMsg_t));

    cJSON * root = cJSON_CreateObject();
    const int size = 16;
    const char *labels[size] = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11",
        "G12", "G13", "G14", "G15", "G16"};

    int maxStep = pStationConfig->I2cExpander.Device.LedCount;
    for (size_t i = 0; i < maxStep; i++) {
        cJSON_AddNumberToObject(root, labels[i], value.gray[i]);
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessWiFiStatusSet(const char *message, const char **output) {
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

esp_err_t ProcessWiFiStatusGet(const char *message, const char **output) {
    if (*output != nullptr)
        return ESP_FAIL;

    char *buffer = new char[256];
    WifiConfig_t config; // Is actually ignored
    *output = "{\"wifiStatus\": \"WiFiParamNotSet\"}";
    if (LoadWiFiConfig(&config) == ESP_OK)
        *output = "{\"wifiStatus\": \"WiFiParamIsSet\"}";
    return ESP_OK;
}
esp_err_t ProcessResetWifiConfig(const char *messange, const char **output) {
    return ResetWiFiConfig();
}


esp_err_t ProcessGetDeviceConfig(const char *message, const char **output) {
    if (*output != nullptr)
        return ESP_FAIL;

    const size_t length = 4096;
    char *buffer = new char[length];
    *output = buffer;

    return Fs_ReadEntry(CONFIG_DEVICE_CONFIGURATION_FILENAME, buffer, length);
}

esp_err_t ProcessSaveToPage(const char *message, const char **output) {
    cJSON *root = cJSON_Parse(message);
    cJSON *e = cJSON_GetObjectItem(root, "Page");
    int page = e->valueint;
    cJSON_Delete(root);

    // @todo
    return ESP_OK;
}

esp_err_t ProcessResetPages(const char *message, const char **output) { return ESP_OK; }
