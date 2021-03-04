/**
 * @file ColorPosts.h
 * @author Gustice
 * @brief 
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 */
#pragma once

#include <stdint.h>
#include "Color.h"

class RgbPost
{
public:
 typedef enum Error {
     colReqOk,
     colReqErr,
 } reqErr_t;

private:
    Color_t * _colors;
    uint16_t _stripLen;
    uint16_t _intensity;
    void ApplyColors(const Color_t &color, const char * apply, uint16_t length);
    void ApplyColorToIndex(const Color_t &color, uint32_t index);
    
    void CreateContainer(const uint16_t len);
public:
    RgbPost(uint16_t length);
    ~RgbPost();

    reqErr_t EvalPost(Color_t &color, uint16_t stripLen, uint16_t intensity, const char * apply);
    const Color_t * GetColors(){ return _colors; }
    uint16_t GetIntensity() {return _intensity; }
    uint16_t GetLength() {return _stripLen; }
};

