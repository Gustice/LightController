#pragma once

#include "MyWeb.h"
#include "Color.h"
#include "ParamReader.h"
#include "SoftAp.h"
#include "esp_http_server.h"

void SetQueueHandlesForPostH(QueueHandle_t colorQ, QueueHandle_t grayQ, pChannelGetCallback getCbk);

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

esp_err_t ProcessRgbiPost(const char *message);
esp_err_t ProcessRgbwPost(const char *message);
esp_err_t ProcessRgbwSinglePost(const char *message);
esp_err_t ProcessGrayValuesPost(const char *message);

esp_err_t ProcessSaveToPage(const char *message);
esp_err_t ProcessResetPages(const char *messange);

esp_err_t ProcessRgbiGet(char *message, char ** output);
esp_err_t ProcessRgbwGet(char *message, char ** output);
esp_err_t ProcessRgbwSingleGet(char *message, char ** output);
esp_err_t ProcessGrayValuesGet(char *message, char ** output);

esp_err_t ProcessWiFiStatusGet(char *message, char **output);
esp_err_t ProcessWiFiStatusSet(const char *message);
esp_err_t ResetWifiConfig(const char *messange);
esp_err_t ProcessGetDeviceConfig(char *message, char **output);

