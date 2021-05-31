/**
 * @file ProcessHandler.cpp
 * @author Gustice
 * @brief Implementation fo Request-Handlers
 * @version 0.1
 * @date 2021-03-04
 *
 * @copyright Copyright (c) 2021
 */

#include "ProcessHandler.h"
#include "Color.h"
#include "MappingParser.h"
#include "MyWeb.h"
#include "WebUtils.h"
#include "cJSON.h"
#include <esp_log.h>
#include <sstream>
#include <string.h>

static const char *cModTag = "Web-ReqPcs";

/* Set HTTP response content type according to file extension */
esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath) {
    const char *type = GetTypeAccordingToExtension(filepath);
    return httpd_resp_set_type(req, type);
}

static QueueHandle_t xSetQuery;
static esp_err_t (*GetChannelSettings)(GetChannelMsg reqest);
static deviceConfig_t *pStationConfig;

void SetQueueHandlesForPostH(
    QueueHandle_t setQuery, pChannelGetCb getCbk, deviceConfig_t *stationConfig) {
    xSetQuery = setQuery;
    GetChannelSettings = getCbk;
    pStationConfig = stationConfig;
}

void SendColorQueue(SetChannelMsg msg, ColorMsg_t color) {
    msg.AdjustTargetIdx(msg.Apply.FirstTarget);
    SetChannelDto_t dto = msg.GetDto();
    ESP_LOGD(cModTag, "Light control: red = %d, green = %d, blue = %d, white = %d, intensity = %d",
        color.red, color.green, color.blue, color.white, color.intensity);

    if (xSetQuery != nullptr) {
        if (xQueueSend(xSetQuery, (void *)&dto, (TickType_t)10) != pdPASS)
            ESP_LOGE(cModTag, "Failed to send the Message");
    } else {
        ESP_LOGE(cModTag, "Queue-Handle not set");
    }
}

void SendGrayValQueue(SetChannelMsg msg, GrayValMsg_t color) {
    msg.AdjustTargetIdx(msg.Apply.FirstTarget);
    SetChannelDto_t dto = msg.GetDto();
    ESP_LOGD(cModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d", color.gray[0],
        color.gray[1], color.gray[2], color.gray[3], color.gray[4], color.gray[5], color.gray[6],
        color.gray[7]);
    ESP_LOGD(cModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d", color.gray[8],
        color.gray[9], color.gray[10], color.gray[11], color.gray[12], color.gray[13],
        color.gray[14], color.gray[15]);

    if (xSetQuery != nullptr) {
        if (xQueueSend(xSetQuery, (void *)&dto, (TickType_t)10) != pdPASS)
            ESP_LOGE(cModTag, "Failed to send the Message");
    } else {
        ESP_LOGE(cModTag, "Queue-Handle not set");
    }
}

struct colorLabels_t {
    const char *name;
    const char **labels;
    uint8_t **values;
    const size_t count;
};

const size_t ColorTypes = 3;
static ColorMsg_t colorMsg;
const char *rgbLabels[3] = {"R", "G", "B"};
uint8_t *rgbValues[3] = {&(colorMsg.red), &(colorMsg.green), &(colorMsg.blue)};
const char *rgbiLabels[4] = {"R", "G", "B", "I"};
uint8_t *rgbiValues[4] = {
    &(colorMsg.red), &(colorMsg.green), &(colorMsg.blue), &(colorMsg.intensity)};
const char *rgbwLabels[4] = {"R", "G", "B", "w"};
uint8_t *rgbwValues[4] = {&(colorMsg.red), &(colorMsg.green), &(colorMsg.blue), &(colorMsg.white)};
static const colorLabels_t Labels[ColorTypes + 1]{
    {"RGB", rgbLabels, rgbValues, 3},
    {"RGBI", rgbiLabels, rgbiValues, 4},
    {"RGBW", rgbwLabels, rgbwValues, 4},
    {"", nullptr, 0},
};

// Payload = {"form: "genericForm", target: "effectCh", type:"RGB", appTo: "1", R: 1, G: 2, B: 3}
esp_err_t ProcessGenericRgbPost(const char *message, const char **output) {
    SetChannelMsg msg(RgbChannel::EffectProcessor, (uint8_t *)&colorMsg);
    cJSON *root = cJSON_Parse(message);
    char *type = cJSON_GetObjectItem(root, "type")->valuestring;

    int idx = 0;
    while (strcmp(Labels[idx].name, type) != 0 && idx < ColorTypes) {
        idx++;
    }
    if (Labels[idx].count == 0) {
        ESP_LOGE(cModTag, "Type %s could not be resolved", type);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < Labels[0].count; i++) {
        cJSON *e = cJSON_GetObjectItem(root, Labels[idx].labels[i]);
        *(Labels[idx].values[i]) = (uint8_t)e->valueint;
    }
    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    uint32_t errors = ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.Apply.Items == 0 && errors == 0)
        msg.Apply.ApplyTo[0] = 1;

    SendColorQueue(msg, colorMsg);
    return ESP_OK;
}

esp_err_t ProcessGenericRgbGet(const char *message, const char **output) {
    GetChannelMsg req(RgbChannel::EffectProcessor, (uint8_t *)&colorMsg);
    ApplyIndexes_t target;

    char buffer[256];
    strcpy(buffer, message);
    //char *type = cJSON_GetObjectItem(root, "type")->valuestring;
    char *type = "RGB"; // Expand route

    ParseApplyToString(buffer, &target);
    req.AdjustTargetIdx(target.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;

    int idx = 0;
    while (strcmp(Labels[idx].name, type) != 0 && idx < ColorTypes) {
        idx++;
    }
    if (Labels[idx].count == 0) {
        ESP_LOGE(cModTag, "Type %s could not be resolved", type);
        return ESP_FAIL;
    }

    cJSON *root = cJSON_CreateObject();
    for (size_t i = 0; i < Labels[0].count; i++) {
        cJSON_AddNumberToObject(root, Labels[0].labels[i], *(Labels[idx].values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

// Payload = {"form":"rgbiSync", "appTo":"1", "R":2, "G":3, "B":4, "I":5}
esp_err_t ProcessRgbiPost(const char *message, const char **output) {
    static ColorMsg_t pColor;
    SetChannelMsg msg(RgbChannel::RgbiSync, (uint8_t *)&pColor);

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.intensity)};

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < size; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }
    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    uint32_t errors = ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.Apply.Items == 0 && errors == 0)
        msg.Apply.ApplyTo[0] = 1;

    SendColorQueue(msg, pColor);
    return ESP_OK;
}

// Payload = {"form":"rgbwAsync", "appTo":"1", "R":2, "G":3, "B":4, "W":6}
esp_err_t ProcessRgbwPost(const char *message, const char **output) {
    static ColorMsg_t pColor;
    SetChannelMsg msg(RgbChannel::RgbwAsync, (uint8_t *)&pColor);

    int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.white)};

    int maxStep = size;
    if (pStationConfig->AsyncLeds.Strip.Channels != ColorChannels::RGBW)
        maxStep = 3;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }

    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    uint32_t errors = ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.Apply.Items == 0 && errors == 0)
        msg.Apply.ApplyTo[0] = 1;

    SendColorQueue(msg, pColor);
    return ESP_OK;
}

// Payload = {"form":"rgbwStrip", "appTo":"1", "R":2, "G":3, "B":4, "W":6}
esp_err_t ProcessRgbwSinglePost(const char *message, const char **output) {
    static ColorMsg_t pColor;
    SetChannelMsg msg(RgbChannel::RgbwPwm, (uint8_t *)&pColor);

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.white)};

    int maxStep = size;
    if (pStationConfig->RgbStrip.Strip.Channels != ColorChannels::RGBW)
        maxStep = 3;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        *(values[i]) = (uint8_t)e->valueint;
    }

    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    uint32_t errors = ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.Apply.Items == 0 && errors == 0)
        msg.Apply.ApplyTo[0] = 1;

    SendColorQueue(msg, pColor);
    return ESP_OK;
}

// Payload = {"form":"grayPort", "appTo":"1", "G1":1,"G2":2,"G3":3,"G4":4,
// "G5":5,"G6":6,"G7":7,"G8":8,
//                "G9":9,"G10":10,"G11":11,"G12":12, "G13":13,"G14":14,"G15":15,"G16":16}
esp_err_t ProcessGrayValuesPost(const char *message, const char **output) {
    static GrayValMsg_t pValues;
    SetChannelMsg msg(RgbChannel::I2cExpanderPwm, (uint8_t *)&pValues);

    const int size = 16;
    const char *labels[size] = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11",
        "G12", "G13", "G14", "G15", "G16"};
    // int values[size];

    int maxStep = pStationConfig->I2cExpander.Device.LedCount;

    cJSON *root = cJSON_Parse(message);
    for (size_t i = 0; i < maxStep; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labels[i]);
        pValues.gray[i] = (uint8_t)e->valueint;
    }
    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    SendGrayValQueue(msg, pValues);

    return ESP_OK;
}

esp_err_t ProcessRgbiGet(const char *message, const char **output) {
    static ColorMsg_t pColor;
    GetChannelMsg req(RgbChannel::RgbiSync, (uint8_t *)&pColor);
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);
    req.AdjustTargetIdx(idx.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;


    cJSON *root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.intensity)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessRgbwGet(const char *message, const char **output) {
    static ColorMsg_t pColor;
    GetChannelMsg req(RgbChannel::RgbwAsync, (uint8_t *)&pColor);
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);
    req.AdjustTargetIdx(idx.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;

    cJSON *root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.white)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessRgbwSingleGet(const char *message, const char **output) {
    static ColorMsg_t pColor;
    GetChannelMsg req(RgbChannel::RgbwPwm, (uint8_t *)&pColor);
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);
    req.AdjustTargetIdx(idx.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;

    cJSON *root = cJSON_CreateObject();
    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "W"};
    uint8_t *values[size] = {&(pColor.red), &(pColor.green), &(pColor.blue), &(pColor.white)};
    for (size_t i = 0; i < size; i++) {
        cJSON_AddNumberToObject(root, labels[i], *(values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t ProcessGrayValuesGet(const char *message, const char **output) {
    static GrayValMsg_t pValues;
    GetChannelMsg req(RgbChannel::I2cExpanderPwm, (uint8_t *)&pValues);
    ApplyIndexes_t idx;

    char buffer[256];
    strcpy(buffer, message);
    ParseApplyToString(buffer, &idx);
    req.AdjustTargetIdx(idx.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;

    cJSON *root = cJSON_CreateObject();
    const int size = 16;
    const char *labels[size] = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11",
        "G12", "G13", "G14", "G15", "G16"};

    int maxStep = pStationConfig->I2cExpander.Device.LedCount;
    for (size_t i = 0; i < maxStep; i++) {
        cJSON_AddNumberToObject(root, labels[i], pValues.gray[i]);
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

// Payload =  {"ssid":"abc","password":"123"}
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

    if (LoadWiFiConfig(&config) == ESP_OK)
        *output = "{\"wifiStatus\": \"WiFiParamIsSet\"}";
    else
        sprintf(buffer, "{\"wifiStatus\": \"WiFiParamNotSet\"}");

    *output = buffer;
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

esp_err_t ProcessGetDeviceType(const char *message, const char **output) {
    if (*output != nullptr)
        return ESP_FAIL;

    const size_t length = 1024;
    char *buffer = new char[length];
    *output = buffer;

    return Fs_ReadEntry(CONFIG_FACTORY_IMAGE_FILENAME, buffer, length);
}

const size_t ImagePagesCount = 4;
const char *ImageFilePattern = "Image%d.txt";


esp_err_t ProcessLoadPage(const char *message, const char **output) {
    cJSON *root = cJSON_Parse(message);
    cJSON *e = cJSON_GetObjectItem(root, "Page");
    int page = e->valueint;
    cJSON_Delete(root);

    if (page >= 4)
        return ESP_FAIL;

    char fileStream[512];
    char fileName[32];
    sprintf(fileName, ImageFilePattern, page);

    if (!Fs_CheckIfExists(fileName)) {
        ESP_LOGW(cModTag, "Page %d not set. Cannot load page.", page);
    }
    ESP_LOGI(cModTag, "Load Current Image to Page %d", page);

    Fs_ReadEntry(fileName, fileStream, sizeof(fileStream));

    static ColorMsg_t pColor[4];
    int channel;

    auto pCn = strtok(fileStream, "\n");
    while ((pCn != NULL) && channel < 4) {
        SetChannelMsg msg(RgbChannel::EffectProcessor, (uint8_t *)&pColor[channel]);
        sscanf(pCn, "Ch%d: R:0x%2x G:0x%2x B:0x%2x W:0x%2x ", &channel,
            (uint32_t *)&pColor[channel].red, (uint32_t *)&(pColor[channel].green),
            (uint32_t *)&(pColor[channel].blue), (uint32_t *)&(pColor[channel].white));

        SendColorQueue(msg, pColor[channel]);
        pCn = strtok(NULL, "\n");
    }

    return ESP_OK;
}

esp_err_t ProcessSaveToPage(const char *message, const char **output) {
    cJSON *root = cJSON_Parse(message);
    cJSON *e = cJSON_GetObjectItem(root, "Page");
    int page = e->valueint;
    cJSON_Delete(root);

    if (page >= 4)
        return ESP_FAIL;

    ESP_LOGI(cModTag, "Save Current Image to Page %d", page);

    static ColorMsg_t pColor;
    GetChannelMsg req(RgbChannel::EffectProcessor, (uint8_t *)&pColor);
    esp_err_t ret;

    char fileStream[512];
    char fileName[32];
    sprintf(fileName, ImageFilePattern, page);

    uint16_t idx = 0;
    req.Target.portIdx = 0;
    ret = GetChannelSettings(req);

    idx += sprintf(&fileStream[idx], "Ch1: R:0x%2x G:0x%2x B:0x%2x W:0x%2x \n", pColor.red,
        pColor.green, pColor.blue, pColor.white);

    req.Target.portIdx = 1;
    ret = GetChannelSettings(req);

    idx += sprintf(&fileStream[idx], "Ch2: R:0x%2x G:0x%2x B:0x%2x W:0x%2x \n", pColor.red,
        pColor.green, pColor.blue, pColor.white);

    ESP_LOGI(cModTag, "Writing Content to File %s: \n%s", fileName, fileStream);
    Fs_SaveEntry(fileName, fileStream, idx);
    return ESP_OK;
}

esp_err_t ProcessResetPages(const char *message, const char **output) {
    char fileName[32];

    for (size_t i = 0; i < ImagePagesCount; i++) {
        sprintf(fileName, ImageFilePattern, i);

        if (Fs_CheckIfExists(fileName)) {
            ESP_LOGI(cModTag, "Deleting file %s", fileName);
            Fs_DeleteEntry(fileName);
        }
    }

    return ESP_OK;
}

esp_err_t ProcessRestartStation(const char *message, const char **output) {
    esp_restart();
    return ESP_OK;
}
