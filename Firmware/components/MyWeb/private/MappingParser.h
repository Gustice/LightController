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

/**
 * @brief Label structure to parse or generate Color-JSON-Messages
 */
struct colorLabels_t {
    /// Color Code, used for assignment
    const char *name;

    /// Labels for parsing or generating JSON-properties
    const char **labels;

    /// Value container for parsed values or to be serialized values
    uint8_t **values;

    /// Number of Labels
    const size_t count;

    /// Container for values
    ColorMsg_t * colorMsg;
};

const colorLabels_t * EvaluateLabelAssignment(char *type);
const char * GetTypeAccordingToExtension(const char *filepath);
ReqColorIdx_t ParseApplyToElement(char *element);
uint32_t ParseApplyToString(char *applyTo, ApplyIndexes_t *indexes);
esp_err_t RgbTargetPathSplitter(char * path, char ** type, char ** index);