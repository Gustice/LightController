#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <string.h>

class PwmPort
{
public:
    uint32_t Value;
    
    PwmPort() {};
    ~PwmPort() {};

    void WritePort(uint32_t duty) {
        Value = duty;
    };
};

