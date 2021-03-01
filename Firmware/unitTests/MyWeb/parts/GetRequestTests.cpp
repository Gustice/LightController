#include "ProcessHandler.h"
#include "TestUtils.h"

static ColorMsg_t NextColorObj;
static GrayValMsg_t NextGrayValObj;
static ReqColorIdx_t LastChTarget;

static esp_err_t GetChannelSettings(ReqColorIdx_t channel, uint8_t * data, size_t length)
{
    if (channel.type == RgbChannel::I2cExpanderPwm)
    {
        memcpy(&LastChTarget, &channel, sizeof(ReqColorIdx_t));
        memcpy(data, &NextGrayValObj, length);
        return ESP_OK;
    }
    
    memcpy(&LastChTarget, &channel, sizeof(ReqColorIdx_t));
    memcpy(data, &NextColorObj, length);
    return ESP_OK;
}

/** "GET /api/GetPort/RGBISync" */
TEST_CASE("Get RGBISync Values Handler-Tests", "[ColorGet]") {
    const char *Payload = "{\"form\":\"rgbiSync\", \"appTo\":\"2.2\"}";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"I\":14}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, nullptr, GetChannelSettings);
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
    const char *Payload = "{\"form\":\"rgbwAsync\", \"appTo\":\"2.2\"}";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"W\":15}";
    
    const char *output;
    SetQueueHandlesForPostH(nullptr, nullptr, GetChannelSettings);
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
    const char *Payload = "{\"form\":\"rgbwStrip\", \"appTo\":\"2.2\"}";
    const char *OutPayload = "{\"R\":11,\"G\":12,\"B\":13,\"W\":15}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, nullptr, GetChannelSettings);
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
    const char *Payload = "{\"form\":\"grayPort\",\"appTo\":\"2.2\"}";
    const char *OutPayload = "{\"G1\":21,\"G2\":22,\"G3\":23,\"G4\":24,\"G5\":25,\"G6\":26,\"G7\":27,\"G8\":28,\"G9\":29,\"G10\":30,\"G11\":31,\"G12\":32,\"G13\":33,\"G14\":34,\"G15\":35,\"G16\":36}";

    const char *output;
    SetQueueHandlesForPostH(nullptr, nullptr, GetChannelSettings);
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

