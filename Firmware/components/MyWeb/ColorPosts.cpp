#include "includes/ColorPosts.h"
#include <string.h>
#include <string>
#include <sstream>

RgbPost::RgbPost(uint16_t length) { CreateContainer(length); }

RgbPost::~RgbPost() { delete[] _colors; }

void RgbPost::CreateContainer(const uint16_t len) {
    _stripLen = len;
    if (len == 0)
        return;

    _colors = new Color_t[len];
    for (size_t i = 0; i < len; i++) {
        _colors[i].red = 0;
        _colors[i].green = 0;
        _colors[i].blue = 0;
        _colors[i].white = 0;
    }
}

RgbPost::reqErr_t RgbPost::EvalPost(Color_t &color, uint16_t stripLen, uint16_t intensity,
                                    const char *apply) {
    if (stripLen != _stripLen) {
        delete[] _colors;
        CreateContainer(_stripLen);
    }

    _intensity = intensity;
    ApplyColors(color, apply, strlen(apply));

    return reqErr_t::colReqOk;
}

/* Syntax:
    - The given number(s) define the index-position on which the color is to be applied
    - All other colors remain in old state
    - Numbers must be given coma-separated 1,2,5 -> Applies color to 1 and 2 and 5
    - No terminating sign nesseccary
    - Numbers can be given as range 1-3,5 -> Applies color to 1 to 3 and 5
    - If a single 'x' or 'X' is given than the color is applied to all elements
*/
void RgbPost::ApplyColors(const Color_t &color, const char *apply, uint16_t length) {
    if (length == 0)
        return;

    if (length == 1) {
        if ((apply[0] == 'x') || (apply[0] == 'X')) {
            for (size_t i = 0; i < _stripLen; i++) {
                memcpy(&_colors[i], &color, sizeof(Color_t));
            }
            return;
        }
    }

    // size_t lPos = length;
    std::string app(apply);
    std::stringstream sstr(apply);
    uint32_t idx = 0;

    size_t cPos = app.find_last_of(',');
    while (cPos != std::string::npos) {
        cPos = app.find_last_of(',');
    }
    cPos = 0;
    
    sstr >> idx;
    // bool success = sstr.good();

    if (idx <= 0) // First index is 1
        return;
    ApplyColorToIndex(color, (uint16_t)--idx);
}

void RgbPost::ApplyColorToIndex(const Color_t &color, uint32_t index) {
    if (index >= _stripLen)
        return;

    _colors[index].red = color.red;
    _colors[index].green = color.green;
    _colors[index].blue = color.blue;
    _colors[index].white = color.white;
}