/**
 * @file ProcessHandler.h
 * @author Gustice
 * @brief API-Process-Handler
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "MyWeb.h"
#include "Color.h"
#include "ParamReader.h"
#include "SoftAp.h"
#include "esp_http_server.h"
#include "DeviceTypes.h"

void SetQueueHandlesForPostH(QueueHandle_t setQuery, pChannelGetCb getCbk, deviceConfig_t * stationConfig);

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

uint32_t ParseApplyToString(char * applyTo, ApplyIndexes_t * idx);

esp_err_t ProcessGenericRgbPost(const char *message, const char **output);
esp_err_t ProcessRgbiPost(const char *message, const char **output);
esp_err_t ProcessRgbwPost(const char *message, const char **output);
esp_err_t ProcessRgbwSinglePost(const char *message, const char **output);
esp_err_t ProcessGrayValuesPost(const char *message, const char **output);

esp_err_t ProcessGenericRgbGet(const char *message, const char ** output);
esp_err_t ProcessRgbiGet(const char *message, const char ** output);
esp_err_t ProcessRgbwGet(const char *message, const char ** output);
esp_err_t ProcessRgbwSingleGet(const char *message, const char ** output);
esp_err_t ProcessGrayValuesGet(const char *message, const char ** output);


esp_err_t ProcessLoadPage(const char *message, const char **output);
esp_err_t ProcessSaveToPage(const char *message, const char **output);
esp_err_t ProcessResetPages(const char *messange, const char **output);


esp_err_t ProcessWiFiStatusGet(const char *message, const char **output);
esp_err_t ProcessGetDeviceConfig(const char *message, const char **output);
esp_err_t ProcessGetDeviceType(const char *message, const char **output);

esp_err_t ProcessWiFiStatusSet(const char *message, const char ** output);
esp_err_t ProcessResetWifiConfig(const char *messange, const char ** output);
esp_err_t ProcessRestartStation(const char *message, const char **output);

