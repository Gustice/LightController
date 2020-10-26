#pragma once

#include <stdint.h>

/**
 * @brief Color Structure
 * @details Defines red/green/blue and white color channel values
 */
typedef struct Color_def {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
} Color_t;


#define NO_COLOR ((Color_t *)0)       ///< Pointer constant to "no color"
#define NO_COLOR_OBJ ((Color *)0)     ///< Null color object
#define USEOLD_COLOR NO_COLOR         ///< Used old color agreement (if no color pointer)
#define USEOLD_COLOR_OBJ NO_COLOR_OBJ ///< Alternative use old color agreement

/**
 * @brief   Color class
 *
 * @details Defines color data and common operations with color objects
 *  \li Supports RGB with overlapping white channel (e.g. SK6812-LED)
 *  \li Supprts Operator overloading with '+' and '*'
 *  \li Return formatted data stream that can be directly sent to an LED
 */
class Color {
    friend Color operator+(Color c1, Color c2);
    friend Color operator*(Color c1, uint8_t k);

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

    Color(Color_t color);

    void SetColor(Color_t color);

    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);

    /**
     * @brief Returns Color as Color_t structure
     * @return Color_t Color
     */
    Color_t GetColor(void) const { return _color; };

    void    WriteByteStreamTo(uint8_t raw[sizeof(Color_t)]);

    // Color operator*(uint8_t k);
    // Color operator+(Color c2);

  private:
    Color_t _color; ///< Color data
};

Color operator*(Color c1, uint8_t k);
Color operator+(Color c1, Color c2);

extern const Color_t color_Black;
extern const Color_t color_Red;
extern const Color_t color_Green;
extern const Color_t color_Blue;
extern const Color_t color_Magenta;
extern const Color_t color_Cyan;
extern const Color_t color_Yellow;
extern const Color_t color_White;
extern const Color_t color_WarmWhite;
extern const Color_t color_ColdWhite;
extern const Color_t color_SpookyWhite;

// @todo this might be useless
extern const Color _ctBlack;
extern const Color _ctRed;
extern const Color _ctGreen;
extern const Color _ctBlue;
extern const Color _ctMagenta;
extern const Color _ctCyan;
extern const Color _ctYellow;
extern const Color _ctWhite;
extern const Color _ctWarmWhite;
extern const Color _ctColdWhite;
extern const Color _ctSpookyWhite;


Color_t led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v);
