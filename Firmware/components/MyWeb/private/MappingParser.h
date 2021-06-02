/**
 * @file MappingParser.h
 * @author Gustice
 * @brief Helper functions and methods for parsing apply index strings
 * @version 0.1
 * @date 2021-05-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "ApiDataTypes.h"
#include "esp_err.h"

const char * GetTypeAccordingToExtension(const char *filepath);
ReqColorIdx_t ParseApplyToElement(char *element);
uint32_t ParseApplyToString(char *applyTo, ApplyIndexes_t *indexes);
esp_err_t RgbTargetPathSplitter(char * path, char ** type, char ** index);