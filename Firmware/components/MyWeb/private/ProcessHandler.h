#pragma once

#include "MyWeb.h"
#include "Color.h"

void SetQueueHandlesForPostH(QueueHandle_t colorQ, QueueHandle_t grayQ);

esp_err_t ProcessRgbiPost(const char *message);
esp_err_t ProcessRgbwPost(const char *message);
esp_err_t ProcessRgbwSinglePost(const char *message);
esp_err_t ProcessGrayValuesPost(const char *message);
esp_err_t ProcessSaveToPage(const char *message);
esp_err_t ProcessResetPages(const char *messange);

esp_err_t ProcessRgbiGet(char *message);
esp_err_t ProcessRgbwGet(char *message);
esp_err_t ProcessRgbwSingleGet(char *message);
esp_err_t ProcessGrayValuesGet(char *message);
