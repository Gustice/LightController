/**
 * @file Color.h
 * @author Gustice
 * @brief   Color-Class and color Definitions
 * @details Defintion of Color Class with Predefined Colors as static ressource
 *
 * @version 0.6
 * @date 2019-09-30
 *
 * @copyright Copyright (c) 2019
 */
#pragma once

#include <stdint.h>
#include <string.h>

/// No Waveform definition
extern uint8_t const *const noWave;

/**
 * @brief Color Structure
 * @details Defines red/green/blue and white color channel values
 */
struct Color_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
};

inline void SetColor(Color_t * tC, Color_t const * sC) {
    tC->red = sC->red;
    tC->green = sC->green;
    tC->blue = sC->blue;
    tC->white = sC->white;
}

/**
 * @brief   Color class
 *
 * @details Defines color data and common operations with color objects
 *  \li Supports RGB with overlapping white channel (e.g. SK6812-LED)
 *  \li Supprts Operator overloading with '+' and '*' (Combined operation with MixWith)
 *  \li Return formatted data stream that can be directly sent to an LED
 */
class Color {
    friend Color operator+(Color c1, Color c2);
    friend Color operator*(Color c1, uint8_t k);

  public:
  private:
    Color_t _color; ///< Color data

  public:
    /**
     * @brief   Construct a new Color object
     * @details Default color will be Black \ref _ctBlack
     */
    Color(void) { SetColor(0, 0, 0, 0); };

    /**
     * @brief   Construct a new Color object according to rgb color-channel-values
     * @details This constructor also takes an overlaping white channel
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @param w White channel (zero by default if not assigned)
     */
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) { SetColor(r, g, b, w); };

    /**
     * @brief Construct a new Color object according to Color_t structure
     * @param color Color to set
     */
    Color(Color_t color) { memcpy(&_color, &color, sizeof(Color_t)); };
    /**
     * @brief Construct a new Color object according to Color_t structure
     * @param color Color to set
     */
    Color(Color_t *color) { memcpy(&_color, color, sizeof(Color_t)); };

    /**
     * @brief Set the Color object according to Color_t Array
     * @param color Color to set
     */
    void SetColor(const Color_t *color) { memcpy(&_color, color, sizeof(Color_t)); }
    /**
     * @brief Set the Color object according to Color_t Array
     * @param color Color to set
     */
    void SetColor(const Color_t color) { memcpy(&_color, &color, sizeof(Color_t)); }

    /**
     * @brief Set the Color object according to rgb color-channel values
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @param w White channel (zero by default if not assigned)
     */
    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) {
        _color.red = r;
        _color.green = g;
        _color.blue = b;
        _color.white = w;
    }

    /**
     * @brief Returns Color as Color_t structure
     * @return Color_t Color
     */
    Color_t GetColor(void) const { return _color; }

    /**
     * @brief Scaling and overlaying with given color.
     * @param c2 Color 2
     * @param k scaling factor
     * @return Overlayed Color
     */
    Color MixWith(const Color c2, uint8_t k = 0x7F) {
        *this = (*this * (UINT8_MAX - k)) + (c2 * k);
        return *this;
    }

    /**
     * @brief Writes Color as formatted bytestream to location
     * @param color
     */
    void WriteByteStreamTo(uint8_t raw[sizeof(Color_t)]) {
        raw[0] = _color.blue;
        raw[1] = _color.green;
        raw[2] = _color.red;
        raw[3] = _color.white;
    }

    /**
     * @brief Assign overload
     */
    Color operator=(Color const *c2) {
        SetColor(c2->_color);
        return *this;
    }

    /**
     * @brief Checks if both color values are equal
     */
    bool operator==(Color const &c2) {
        return ((_color.blue == c2._color.blue) && (_color.green == c2._color.green) &&
                (_color.red == c2._color.red) && (_color.white == c2._color.white));
    }

    /**
     * @brief Checks if both color values are not equal
     */
    bool operator!=(Color const &c2) {
        return ((_color.blue != c2._color.blue) || (_color.green != c2._color.green) ||
                (_color.red != c2._color.red) || (_color.white != c2._color.white));
    }
};

/**
 * @brief Scale color by scaling each color-channel by given factor
 * @param c1 Color
 * @param k Scale-factor uint8_t.Max is fullscale
 * @return Scaled Color
 */
Color operator*(Color c1, uint8_t k);

/**
 * @brief Overlaying to colors my adding each color channel value
 * @note Colors channels should not overlap since there is no automatic ratio correction.
 *    Use *-Operator first in order to scale each color value a a way that no overflow can occur
 * @param c1 Color 1
 * @param c2 Color 2
 * @return Overlayed Color
 */
Color operator+(Color c1, Color c2);

/// Pointer constant to "no color"
extern Color_t const *const noColor;
/// Pointer constant to "no color"
extern Color_t const *const oldColor;

/// Null color object
extern const Color &noColorObj;
/// Used old color agreement (if no color pointer)
extern const Color &oldColorObj;

/// Black color structure constant
extern const Color_t color_Black;
/// Red color structure constant
extern const Color_t color_Red;
/// Green color structure constant
extern const Color_t color_Green;
/// Blue color structure constant
extern const Color_t color_Blue;
/// Magenta color structure constant
extern const Color_t color_Magenta;
/// Cyan color structure constant
extern const Color_t color_Cyan;
/// Yellow color structure constant
extern const Color_t color_Yellow;
/// White color structure constant
extern const Color_t color_White;
/// Warm white color structure constant (with higher red channel value)
extern const Color_t color_WarmWhite;
/// Black color structure constant (with higher blue channel value)
extern const Color_t color_ColdWhite;
/// Black color structure constant (with higher green channel value)
extern const Color_t color_SpookyWhite;

/// Black color object constant
extern const Color CBlack;
/// Red color object constant
extern const Color CRed;
/// Green color object constant
extern const Color CGreen;
/// Blue color object constant
extern const Color CBlue;
/// Magenta color object constant
extern const Color CMagenta;
/// Cyan color object constant
extern const Color CCyan;
/// Yellow color object constant
extern const Color CYellow;
/// White color object constant
extern const Color CWhite;
/// Warm white color object constant (with higher red channel value)
extern const Color CWarmWhite;
/// Black color object constant (with higher green channel value)
extern const Color CColdWhite;
/// Black color object constant (with higher blue channel value)
extern const Color CSpookyWhite;
