#pragma once 

#include <string.h>
#include <stdint.h>

class Pca9685 {
  public:
    const size_t Length; 
    Pca9685(size_t len) : Length(len) {};
    ~Pca9685();

    uint16_t buffer[64];

    void SendImage(uint16_t * grayValues) {
        memcpy(buffer, grayValues, sizeof(uint16_t) * Length);
    };
    void Reset() {};
};
