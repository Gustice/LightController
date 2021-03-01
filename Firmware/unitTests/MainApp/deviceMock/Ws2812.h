#pragma once 

#include <string.h>
#include "Color.h"

class Ws2812 {
  public:
    const size_t Length; 
    Ws2812(size_t len) : Length(len) {};
    ~Ws2812();

    Color_t buffer[64];

    void SendImage(Color_t *colors) { 
        memcpy(buffer, colors, sizeof(Color_t)* Length);
    }
};