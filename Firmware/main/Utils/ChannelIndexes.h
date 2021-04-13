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
 *
 */
class ChannelIndexes {
  public:
    const size_t Count;
    const size_t ImageSize;
    const Color_t **colorPool;
    Color_t *Image;
    RotatingIndex Led;
    RotatingIndex Color;

    ChannelIndexes(const size_t ledCount, const Color_t **pool, const size_t colorCount)
        : Count(ledCount), ImageSize(sizeof(Color_t) * ledCount), colorPool(pool), Led(ledCount),
          Color(colorCount) {
        Image = new Color_t[ledCount];
        ClearImage();
    };
    ~ChannelIndexes() { delete[] Image; };

    void ClearImage(void) { memset(Image, 0, ImageSize); }

    uint16_t SetNextSlotMindOverflow() {
        uint16_t idx = Led.GetIndex();
        memcpy(&Image[idx], colorPool[Color.GetIndex()], sizeof(Color_t));

        if (Led.PeekRevolution()) {
            Color.GetIndexAndInkrement();
        }
        Led.GetIndexAndInkrement();

        return idx;
    }

    void SetImage(Color_t * pColor)
    {
        for (size_t i = 0; i < Count; i++) {
            memcpy(&Image[i], pColor, sizeof(Color_t));
        }
    }
};