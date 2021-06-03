/**
 * @file Color.cpp
 * @author Gustice
 * @brief Implementation of Color-Class Color.h
 * @version 0.6
 * @date 2019-09-30
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "Color.h"
#include <string.h>

uint8_t const * const noWave = nullptr;
Color_t const * const noColor = nullptr;
Color_t const * const oldColor = nullptr; 

const Color_t color_Black = {0x00, 0x00, 0x00, 0x00};
const Color_t color_Red = {0xFF, 0x00, 0x00, 0x00};
const Color_t color_Green = {0x00, 0xFF, 0x00, 0x00};
const Color_t color_Blue = {0x00, 0x00, 0xFF, 0x00};
const Color_t color_Magenta = {0x80, 0x00, 0x80, 0x00};
const Color_t color_Cyan = {0x00, 0x80, 0x80, 0x00};
const Color_t color_Yellow = {0x80, 0x80, 0x00, 0x00};
const Color_t color_White = {0x55, 0x55, 0x55, 0x00};
const Color_t color_WarmWhite = {0x7F, 0x4F, 0x4F, 0x00};
const Color_t color_ColdWhite = {0x4F, 0x4F, 0x7F, 0x00};
const Color_t color_SpookyWhite = {0x4F, 0x7F, 0x4F, 0x00};

const Color CBlack(color_Black);
const Color CRed(color_Red);
const Color CGreen(color_Green);
const Color CBlue(color_Blue);
const Color CMagenta(color_Magenta);
const Color CCyan(color_Cyan);
const Color CYellow(color_Yellow);
const Color CWhite(color_White);
const Color CWarmWhite(color_WarmWhite);
const Color CColdWhite(color_ColdWhite);
const Color CSpookyWhite(color_SpookyWhite);
///@}

Color operator*(Color c1, uint8_t k) {
    c1._color.red   = (uint8_t)((uint16_t)c1._color.red * k / 0xFF);
    c1._color.green = (uint8_t)((uint16_t)c1._color.green * k / 0xFF);
    c1._color.blue  = (uint8_t)((uint16_t)c1._color.blue * k / 0xFF);
    c1._color.white = (uint8_t)((uint16_t)c1._color.white * k / 0xFF);
    return c1;
}

Color operator+(Color c1, Color c2) {
    c1._color.red   = c1._color.red + c2._color.red;
    c1._color.green = c1._color.green + c2._color.green;
    c1._color.blue  = c1._color.blue + c2._color.blue;
    c1._color.white = c1._color.white + c2._color.white;
    return c1;
}
