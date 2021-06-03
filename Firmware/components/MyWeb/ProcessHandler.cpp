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

void SendColorQueue(SetChannelMsg msg) {
    ColorMsg_t *pColor = (ColorMsg_t *)msg.pStream;
    msg.AdjustTargetIdx(msg.Apply.FirstTarget);
    SetChannelDto_t dto = msg.GetDto();
    ESP_LOGI(cModTag, "Light control: red = %d, green = %d, blue = %d, white = %d, intensity = %d",
        pColor->red, pColor->green, pColor->blue, pColor->white, pColor->intensity);
    ESP_LOGI(cModTag, "Target: %d.%d", msg.Apply.FirstTarget.chIdx, msg.Apply.FirstTarget.portIdx);

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

// Payload = {"form: "genericForm", target: "effectCh", type:"RGB", appTo: "1", R: 1, G: 2, B: 3}
esp_err_t ProcessGenericRgbPost(const char *message, const char **output) {
    cJSON *root = cJSON_Parse(message);
    char *type = cJSON_GetObjectItem(root, "type")->valuestring;

    const colorLabels_t *labelSet = EvaluateLabelAssignment(type);
    if (labelSet == nullptr)
        return ESP_FAIL;
    SetChannelMsg msg(RgbChannel::EffectProcessor, (uint8_t *)labelSet->colorMsg);

    for (size_t i = 0; i < labelSet->count; i++) {
        cJSON *e = cJSON_GetObjectItem(root, labelSet->labels[i]);
        *(labelSet->values[i]) = (uint8_t)e->valueint;
    }
    char *apply = cJSON_GetObjectItem(root, "appTo")->valuestring;
    uint32_t errors = ParseApplyToString(apply, &msg.Apply);
    cJSON_Delete(root);

    // Apply allways to first item if no items are Set and nor errors occurred
    if (msg.Apply.Items == 0 && errors == 0)
        msg.Apply.ApplyTo[0] = 1;

    SendColorQueue(msg);
    return ESP_OK;
}

// Payload "RGB/1"
esp_err_t ProcessGenericRgbGet(const char *message, const char **output) {
    ApplyIndexes_t target;

    char buffer[256];
    strcpy(buffer, message);
    char *trgStr;   // Something like RgbChannel::RgbiSync
    char *type;     // Something like RGB
    char *applyStr; // Something like 1 or 1.1
    if (ESP_OK != RgbTargetPathSplitter(buffer, &type, &applyStr))
        return ESP_FAIL;

    const colorLabels_t *labelSet = EvaluateLabelAssignment(type);
    if (labelSet == nullptr)
        return ESP_FAIL;
    GetChannelMsg req(RgbChannel::EffectProcessor, (uint8_t *)labelSet->colorMsg);

    ParseApplyToString(applyStr, &target);
    req.AdjustTargetIdx(target.FirstTarget);
    if (GetChannelSettings(req) != ESP_OK)
        return ESP_FAIL;

    cJSON *root = cJSON_CreateObject();
    for (size_t i = 0; i < labelSet->count; i++) {
        cJSON_AddNumberToObject(root, labelSet->labels[i], *(labelSet->values[i]));
    }
    *output = cJSON_PrintUnformatted(root); // to save pretty whitespaces cJSON_Print
    cJSON_Delete(root);
    return ESP_OK;
}

// Payload = {"form":"rgbiSync", "appTo":"1", "R":2, "G":3, "B":4, "I":5}
esp_err_t ProcessRgbiPost(const char *message, const char **output) {
    static ColorMsg_t color;
    SetChannelMsg msg(RgbChannel::RgbiSync, (uint8_t *)&color);

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", "I"};
    uint8_t *values[size] = {&(color.red), &(color.green), &(color.blue), &(color.intensity)};

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

    SendColorQueue(msg);
    return ESP_OK;
}

// Payload = {"form":"rgbwAsync", "appTo":"1", "R":2, "G":3, "B":4, "W":6}
esp_err_t ProcessRgbwPost(const char *message, const char **output) {
    static ColorMsg_t color;
    SetChannelMsg msg(RgbChannel::RgbwAsync, (uint8_t *)&color);

    int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(color.red), &(color.green), &(color.blue), &(color.white)};

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

    SendColorQueue(msg);
    return ESP_OK;
}

// Payload = {"form":"rgbwStrip", "appTo":"1", "R":2, "G":3, "B":4, "W":6}
esp_err_t ProcessRgbwSinglePost(const char *message, const char **output) {
    static ColorMsg_t color;
    SetChannelMsg msg(RgbChannel::RgbwPwm, (uint8_t *)&color);

    const int size = 4;
    const char *labels[size] = {"R", "G", "B", /*optional*/ "W"};
    uint8_t *values[size] = {&(color.red), &(color.green), &(color.blue), &(color.white)};

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

    SendColorQueue(msg);
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
        sprintf(buffer, "{\"wifiStatus\": \"WiFiParamIsSet\"}");
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

    if ((page == 0) || (page > 4))
        return ESP_FAIL;

    char fileStream[512];
    char fileName[32];
    sprintf(fileName, ImageFilePattern, page);

    if (ESP_OK == Fs_CheckIfExists(fileName)) {
        ESP_LOGW(cModTag, "Page %d not set. Cannot load page.", page);
    }
    ESP_LOGI(cModTag, "Load Current Image to Page %d", page);

    Fs_ReadEntry(fileName, fileStream, sizeof(fileStream));

    static ColorMsg_t pColor[4];
    int channel;

    auto pCn = strtok(fileStream, "\n");
    while ((pCn != NULL) && channel < 4) {
        Color_t temp;
        sscanf(pCn, "Ch %d: R:0x%02x G:0x%02x B:0x%02x W:0x%02x", &channel,
            (uint32_t *)&(temp.red), (uint32_t *)&(temp.green),
            (uint32_t *)&(temp.blue), (uint32_t *)&(temp.white));
        pColor[channel].red = temp.red;
        pColor[channel].green = temp.green;
        pColor[channel].blue = temp.blue;
        pColor[channel].white = temp.white;
        SetChannelMsg msg(RgbChannel::EffectProcessor, (uint8_t *)&(pColor[channel]));
        ReqColorIdx_t rq = {
            .type = RgbChannel::EffectProcessor, .chIdx = 0, .portIdx = (uint16_t)channel};
        msg.AdjustFirstTargetIdx(rq);
        SendColorQueue(msg);
        pCn = strtok(NULL, "\n");
    }
    return ESP_OK;
}

esp_err_t ProcessSaveToPage(const char *message, const char **output) {
    cJSON *root = cJSON_Parse(message);
    cJSON *e = cJSON_GetObjectItem(root, "Page");
    int page = e->valueint;
    cJSON_Delete(root);

    if ((page == 0) || (page > 4))
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

    idx += sprintf(&fileStream[idx], "Ch 0: R:0x%02x G:0x%02x B:0x%02x W:0x%02x \n", pColor.red,
        pColor.green, pColor.blue, pColor.white);

    req.Target.portIdx = 1;
    ret = GetChannelSettings(req);

    idx += sprintf(&fileStream[idx], "Ch 1: R:0x%02x G:0x%02x B:0x%02x W:0x%02x \n", pColor.red,
        pColor.green, pColor.blue, pColor.white);

    ESP_LOGI(cModTag, "Writing Content to File %s: \n%s", fileName, fileStream);
    Fs_SaveEntry(fileName, fileStream, idx);
    return ESP_OK;
}

esp_err_t ProcessResetPages(const char *message, const char **output) {
    char fileName[32];
    bool deleted = false;
    for (size_t i = 0; i < ImagePagesCount; i++) {
        sprintf(fileName, ImageFilePattern, i + 1);

        if (ESP_OK == Fs_CheckIfExists(fileName)) {
            ESP_LOGI(cModTag, "Deleting file %s", fileName);
            Fs_DeleteEntry(fileName);
            deleted = true;
        }
    }

    if (deleted == false)
        ESP_LOGI(cModTag, "Nothing to delete");

    return ESP_OK;
}

esp_err_t ProcessRestartStation(const char *message, const char **output) {
    esp_restart();
    return ESP_OK;
}
