#include "ProcessHandler.h"
#include "TestUtils.h"

static ColorMsg_t NextColorObj;
static GrayValMsg_t NextGrayValObj;
static ReqColorIdx_t LastChTarget;

static esp_err_t GetChannelSettings(GetChannelMsg request)
{
    if (request.Target.type == RgbChannel::I2cExpanderPwm)
    {
        memcpy(&LastChTarget, &request.Target, sizeof(ReqColorIdx_t));
        memcpy(request.pStream, &NextGrayValObj, request.PayLoadSize);
        return ESP_OK;
    }
    
    memcpy(&LastChTarget, &request.Target, sizeof(ReqColorIdx_t));
    memcpy(request.pStream, &NextColorObj, request.PayLoadSize);
    return ESP_OK;
}

static deviceConfig_t global_config {
    .SyncLeds {
        .Strip {
            .Channels = ColorChannels::RGBW,
        }
    },
    .AsyncLeds {
        .Strip {
            .Channels = ColorChannels::RGBW,
        }
    },
    .RgbStrip {
        .Strip {
            .Channels = ColorChannels::RGBW,
        }
    },
    .I2cExpander {
        .Device {
            .LedCount = 16,
        }
    }
};

/** "GET /api/GetPort/RGBISync" */
TEST_CASE("Get RGBISync Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "2.2";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"I\":14}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, GetChannelSettings, &global_config);
    NextColorObj.red = 11;
    NextColorObj.green = 12;
    NextColorObj.blue = 13;
    NextColorObj.intensity = 14;

    bool result = ProcessRgbiGet(Payload, &output);
    INFO(" Return string is: " << output);
    INFO("Compare string is: " << OutPayload);
    CHECK( (strcmp(output, OutPayload) == 0) );
    CHECK( (LastChTarget.chIdx == 1) );
    CHECK( (LastChTarget.portIdx == 1) );
    // delete[] OutPayload;
}

/** "GET /api/GetPort/RGBWAsync" */
TEST_CASE("Get RGBWAsync Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "2.2";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"W\":15}";
    
    const char *output;
    SetQueueHandlesForPostH(nullptr, GetChannelSettings, &global_config);
    NextColorObj.red = 11;
    NextColorObj.green = 12;
    NextColorObj.blue = 13;
    NextColorObj.white = 15;
    
    bool result = ProcessRgbwGet(Payload, &output);
    
    INFO(" Return string is: " << output);
    INFO("Compare string is: " << OutPayload);
    CHECK( (strcmp(output, OutPayload) == 0) );
    CHECK( (LastChTarget.chIdx == 1) );
    CHECK( (LastChTarget.portIdx == 1) );
    delete[] OutPayload;
}

/** "GET /api/GetPort/RGBWSingle" */
TEST_CASE("Get RGBWSingle Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "2.2";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"W\":15}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, GetChannelSettings, &global_config);
    NextColorObj.red = 11;
    NextColorObj.green = 12;
    NextColorObj.blue = 13;
    NextColorObj.white = 15;

    bool result = ProcessRgbwSingleGet(Payload, &output);
    INFO(" Return string is: " << output);
    INFO("Compare string is: " << OutPayload);
    CHECK( (strcmp(output, OutPayload) == 0) );
    CHECK( (LastChTarget.chIdx == 1) );
    CHECK( (LastChTarget.portIdx == 1) );
    delete[] OutPayload;
}

/** "GET /api/GetPort/IValues" */
TEST_CASE("Get IValues Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "2.2";
    const char *OutPayload = "{\"G1\":21,\"G2\":22,\"G3\":23,\"G4\":24,\"G5\":25,\"G6\":26,\"G7\":27,\"G8\":28,\"G9\":29,\"G10\":30,\"G11\":31,\"G12\":32,\"G13\":33,\"G14\":34,\"G15\":35,\"G16\":36}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, GetChannelSettings, &global_config);
    for (size_t i = 0; i < 16; i++) {
        NextGrayValObj.gray[i] = 21 + i;
    }

    bool result = ProcessGrayValuesGet(Payload, &output);
    INFO(" Return string is: " << output);
    INFO("Compare string is: " << OutPayload);
    CHECK( (strcmp(output, OutPayload) == 0) );
    CHECK( (LastChTarget.chIdx == 1) );
    CHECK( (LastChTarget.portIdx == 1) );
    delete[] OutPayload;
}



/** "GET /api/GetPort/genericRGB" */
TEST_CASE("Get genericRGB Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "RGB/2";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, GetChannelSettings, &global_config);
    NextColorObj.red = 11;
    NextColorObj.green = 12;
    NextColorObj.blue = 13;
    NextColorObj.white = 15;

    bool result = ProcessGenericRgbGet(Payload, &output);
    INFO(" Return string is: " << output);
    INFO("Compare string is: " << OutPayload);
    CHECK( (strcmp(output, OutPayload) == 0) );
    CHECK( (LastChTarget.chIdx == 0) );
    CHECK( (LastChTarget.portIdx == 1) );
    delete[] OutPayload;
}