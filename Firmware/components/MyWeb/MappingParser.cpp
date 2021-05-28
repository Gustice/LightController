/**
 * @file MappingParser.cpp
 * @author Gustice
 * @brief Implementation of Parser
 * @version 0.1
 * @date 2021-05-27
 * 
 * @copyright Copyright (c) 2021
 */

#include "MappingParser.h"
#include <sstream>
#include <string.h>

#define CHECK_FILE_EXTENSION(filename, ext)                                                        \
    (strcmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

const char * GetTypeAccordingToExtension(const char *filepath) {
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
    return type;
}


bool charInSpan(char c, char from, char to) {
  return  ((c >= from) && (c <= to));
}

bool charIsumber(char c) {
  return charInSpan(c, '0', '9');
}

bool charIsNotNumber(char c) {
  return  !charInSpan(c, '0', '9');
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

        // Indexes are 1-Based
        if ((ch <= 0) || (ch > ApplyToTargetChannels))
            return idx;
        if ((pos <= 0) || (pos > ApplyToChannelWidth))
            return idx;

        // Correct indexes to 0-Based
        ch -= 1;
        pos -= 1;
    } else {
        int port;
        port = std::stoi(element);

        if (port <= 0)
            return idx;
        if (port > ApplyToTargetChannels * ApplyToChannelWidth)
            return idx;

        port -= 1;
        ch = port / ApplyToChannelWidth;
        pos = port % ApplyToChannelWidth;
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
 *  Spans are also possible '*' applies to all, 1-3 applies to 1 to 3 inclusive
 *
 * @param applyTo list of indexes to parse and translate to index-variable
 * @param indexes container to place found indexes in
 */
uint32_t ParseApplyToString(char *applyTo, ApplyIndexes_t *indexes) {
    // @todo Exception handling badly needed :-/
    uint32_t errors = 0;
    if (applyTo == nullptr || indexes == nullptr)
        return ++errors;

    size_t len = strlen(applyTo);
    if (len == 0)
        return errors;

    if (len == 1) {
        if (applyTo[0] == '*') {
            for (size_t i = 0; i < ApplyToTargetChannels; i++) {
                indexes->ApplyTo[i] = 0xFFFFFFFF;
                indexes->Items += 32;
            }
            return errors;
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

    // Split list on next comma
    pCn = strtok(applyTo, ","); 

    while (pCn != NULL) { // While comma is present
        pC = pCn;
        // Split list on next comma
        pCn = strtok(NULL, ","); 

        pR = strchr(pC, '-');
        if (pR != NULL) { // '-'
            char *pPre = pR - 1;
            // Search for Number befor dash
            while (pPre >= pC && (*pPre < '0') && (*pPre > '9')) {
                pPre--;
            }

            if (pPre < pC) // On limit no number between delimiter and minus
            { // negative numbers are not accepted
                errors++;
                continue;
            }

            idx = ParseApplyToElement(pC);
            if (idx.type == RgbChannel::None_Error) {
                errors++;
                continue;
            }

            ReqColorIdx_t toIdx = ParseApplyToElement(pR + 1);
            if (toIdx.type == RgbChannel::None_Error) {
                errors++;
                continue;
            }

            size_t from = ApplyToChannelWidth * idx.chIdx + idx.portIdx;
            size_t to = ApplyToChannelWidth * toIdx.chIdx + toIdx.portIdx;

            if (from >= to) {
                errors++;
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
                errors++;
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

    return errors;
}