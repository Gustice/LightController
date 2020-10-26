/**
 * @file RgbwStrip.cpp
 * @author Gustice
 * @brief Rgbw-LED-Strips-Class implementation
 * @version 0.1
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "RgbwStrip.h"

RgbwStrip::RgbwStrip(PwmPort *red, PwmPort *green, PwmPort *blue, PwmPort *white) {
    assert(red != nullptr);
    assert(green != nullptr);
    assert(blue != nullptr);
    _r = red;
    _g = green;
    _b = blue;
    _w = white;
};

void RgbwStrip::SetImage(const Color_t *colors) {
    _r->WritePort(colors->red);
    _g->WritePort(colors->green);
    _b->WritePort(colors->blue);
    if (_w != nullptr)
        _w->WritePort(colors->white);
}
