/**
 * @file ChannelIndexes.h
 * @author Gustice
 * @brief Data object for lighting port
 * @version 0.1
 * @date 2021-03-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include "Color.h"
#include "RotatingIndex.h"
#include <string.h>

/**
 * @brief Data class for lighting port data
 * @details Concentrates all references to buffer and current color indexes
 */
class ChannelIndexes {
  public:
    /// LED count
    const size_t Count;

    /// Length of binary color image size (i.e. Count * sizeof(Color_t))
    const size_t ImageSize;

    /// Reference to array of colors that can be used in an automatic revolving manner
    const Color_t **colorPool;

    /// Color image that is used to be written on LED strip
    Color_t *Image;

    /// Index to currently active LED
    RotatingIndex Led;

    /// Index to currently set color
    RotatingIndex Color;

    /**
     * @brief Construct
     * @param ledCount LED count in target LED strip
     * @param pool Color pool for automatic color switching
     * @param colorCount
     */
    ChannelIndexes(const size_t ledCount, const Color_t **pool, const size_t colorCount)
        : Count(ledCount), ImageSize(sizeof(Color_t) * ledCount), colorPool(pool), Led(ledCount),
          Color(colorCount) {
        Image = new Color_t[ledCount];
        ClearImage();
    };
    ~ChannelIndexes() { delete[] Image; };

    /**
     * @brief Set whole Image to dark (all LEDs will be turned of on next refresh)
     */
    void ClearImage(void) { memset(Image, 0, ImageSize); }

    /**
     * @brief Get next index for moving dot (with overflow)
     * @details Active color dot moves across LED strip
     * @return uint16_t
     */
    uint16_t GetNextSlot() {
        uint16_t idx = Led.GetIndex();
        memcpy(&Image[idx], colorPool[Color.GetIndex()], sizeof(Color_t));

        if (Led.PeekRevolution()) {
            Color.GetIndexAndInkrement();
        }
        Led.GetIndexAndInkrement();

        return idx;
    }

    /**
     * @brief Set all pixels of an image to given color
     * @param pColor set color
     */
    void SetImage(Color_t *pColor) {
        for (size_t i = 0; i < Count; i++) {
            memcpy(&Image[i], pColor, sizeof(Color_t));
        }
    }
};