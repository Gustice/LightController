#pragma once

#include <string.h>
#include "Color.h"

class Apa102 {
  public:
    const size_t Length; 
    
    Apa102(size_t len) : Length(len) {};
    ~Apa102(){};

    Color_t buffer[64];

    void SendImage(Color_t *colors) { 
        memcpy(buffer, colors, sizeof(Color_t)* Length);
    }

    void SetBrightness(uint8_t bright) {};
};
