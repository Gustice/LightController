#pragma once 

#include <string.h>
#include "Color.h"

class RgbwStrip {
  public:
  const size_t Length; 
    RgbwStrip(size_t len) : Length(len) {};
    ~RgbwStrip(){};

    Color_t buffer[8];

    void SetImage(Color_t *colors) { 
        memcpy(buffer, colors, sizeof(Color_t)* Length);
    }
};