/**
 * @file ApiDataTypes.h
 * @author Gustice
 * @brief General definition of datatypes used accross project
 * @version 0.1
 * @date 2021-03-04
 *
 * @copyright Copyright (c) 2021
 */
#pragma once

#include <stdint.h>
#include <string.h>
#include <Color.h>

typedef enum RgbChannel {
    None_Error = -1,
    Undefined = 0,
    RgbiSync,        // Only on Channel supported
    RgbwAsync,       // Only one Channel supported
    RgbwPwm,         // May be multiple channels
    I2cExpanderPwm,  // May be multiple channels
    EffectProcessor, // Normally multiple channels, drives some of the above
    LastChannel
} eRgbChannel_t;

struct ReqColorIdx_t {
    RgbChannel type;
    int16_t chIdx;
    int16_t portIdx;
};

#define ApplyToTargetChannels 5
#define ApplyToChannelWidth   32
struct ApplyIndexes_t {
    uint16_t Items;
    ReqColorIdx_t FirstTarget;
    uint32_t ApplyTo[ApplyToTargetChannels];
};

struct ColorMsg_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t intensity;
};

inline void CopyColorToColorMessage(ColorMsg_t * msg, Color_t * color){
    msg->red = color->red;
    msg->green = color->green;
    msg->blue = color->blue;
    msg->white = color->white;
}
inline void CopyColorMessageToColor(Color_t * color, ColorMsg_t * msg){
    color->red = msg->red;
    color->green = msg->green;
    color->blue = msg->blue;
    color->white = msg->white;
}

struct GrayValMsg_t {
    uint8_t gray[16];
    uint8_t intensity;
};

class ChannelMsg {
  public:
    size_t PayLoadSize;
    uint8_t *const pStream;
    ReqColorIdx_t Target;

    ChannelMsg(RgbChannel type, uint8_t *stream) : pStream(stream) {
        Target.type = type;
        Target.chIdx = 0;
        Target.portIdx = 0;

        switch (type) {
        case RgbChannel::RgbiSync:
        case RgbChannel::RgbwAsync:
        case RgbChannel::RgbwPwm:
        case RgbChannel::EffectProcessor:
            PayLoadSize = sizeof(ColorMsg_t);
            break;

        case RgbChannel::I2cExpanderPwm:
            PayLoadSize = sizeof(GrayValMsg_t);
            break;

        default:
            PayLoadSize = 0;
            Target.type = RgbChannel::None_Error;
        }
    };
    ~ChannelMsg(){};

    size_t GetSize(void) { return PayLoadSize; };

    void AdjustTargetIdx(ReqColorIdx_t target) {
        Target.chIdx = target.chIdx;
        Target.portIdx = target.portIdx;
    }
};

class GetChannelMsg : public ChannelMsg {
  public:
    GetChannelMsg(RgbChannel type, uint8_t *pStream) : ChannelMsg(type, pStream){};
    ~GetChannelMsg(){};
};


struct SetChannelDto_t {
    ReqColorIdx_t Target;
    uint8_t *pStream;
    size_t PayLoadSize;
    ApplyIndexes_t Apply;
};

class SetChannelMsg : public ChannelMsg {
  public:
    ApplyIndexes_t Apply;
    SetChannelMsg(RgbChannel type, uint8_t *pStream) : ChannelMsg(type, pStream) {
        Apply.Items = 0;
        Apply.FirstTarget.type = type;
        Apply.FirstTarget.chIdx = 0;
        Apply.FirstTarget.portIdx = 0;
        for (size_t i = 0; i < ApplyToTargetChannels; i++) {
            Apply.ApplyTo[i] = 0;
        }
    };
    ~SetChannelMsg(){};

    SetChannelDto_t GetDto(void) {
        SetChannelDto_t dto = {.Target{
                                   .type = Target.type,
                                   .chIdx = Target.chIdx,
                                   .portIdx = Target.portIdx,
                               },
            .pStream = pStream,
            .PayLoadSize = PayLoadSize};
        memcpy(&dto.Apply, &Apply, sizeof(ApplyIndexes_t));
        return dto;
    }
};
