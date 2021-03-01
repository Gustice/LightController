#pragma once

#include "Color.h"
#include "RotatingIndex.h"
#include <string.h>

class ChannelIndexes {
  public:
    const size_t ImageSize;
    const Color_t **colorPool;
    Color_t *Image;
    RotatingIndex Led;
    RotatingIndex Color;

    ChannelIndexes(const size_t ledCount, const Color_t **pool, const size_t colorCount)
        : ImageSize(sizeof(Color_t) * ledCount), colorPool(pool), Led(ledCount), Color(colorCount) {
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
};