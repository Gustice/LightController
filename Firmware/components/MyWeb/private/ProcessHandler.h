#pragma once

#include "MyWeb.h"
#include "Color.h"
#include "ParamReader.h"
#include "SoftAp.h"
#include "esp_http_server.h"

void SetQueueHandlesForPostH(QueueHandle_t colorQ, QueueHandle_t grayQ, pChannelGetCallback getCbk);

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

void ParseApplyToString(char * applyTo, ApplyIndexes_t * idx);

esp_err_t ProcessRgbiPost(const char *message, const char **output);
esp_err_t ProcessRgbwPost(const char *message, const char **output);
esp_err_t ProcessRgbwSinglePost(const char *message, const char **output);
esp_err_t ProcessGrayValuesPost(const char *message, const char **output);

esp_err_t ProcessSaveToPage(const char *message, const char **output);
esp_err_t ProcessResetPages(const char *messange, const char **output);

esp_err_t ProcessRgbiGet(char *message, const char ** output);
esp_err_t ProcessRgbwGet(char *message, const char ** output);
esp_err_t ProcessRgbwSingleGet(char *message, const char ** output);
esp_err_t ProcessGrayValuesGet(char *message, const char ** output);

esp_err_t ProcessWiFiStatusGet(char *message, const char **output);
esp_err_t ProcessWiFiStatusSet(const char *message, const char ** output);
esp_err_t ProcessResetWifiConfig(const char *messange, const char ** output);
esp_err_t ProcessGetDeviceConfig(char *message, const char **output);

