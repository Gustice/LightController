#include "ProcessHandler.h"
#include "TestUtils.h"

extern SetChannelDto_t LastSetMsg;

TEST_CASE("Evaluate String Functions again (problems with compiler)", "[Basics]") {
    CHECK(2 == std::stoi("2"));
    CHECK(3 == std::stoi(" 3"));
    CHECK(4 == std::stoi("4 "));

    CHECK(2 == std::stoi("2.0"));
    CHECK(3 == std::stoi(" 3  . 1"));
    CHECK(4 == std::stoi("4 .3   "));
}

static deviceConfig_t global_config{.SyncLeds{.Strip{
                                        .Channels = ColorChannels::RGBW,
                                    }},
    .AsyncLeds{.Strip{
        .Channels = ColorChannels::RGBW,
    }},
    .RgbStrip{.Strip{
        .Channels = ColorChannels::RGBW,
    }},
    .I2cExpander{.Device{
        .LedCount = 16,
    }}};
    
/** "POST /api/SetPort/RGBISync" */
TEST_CASE("Post RGBISync Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbiSync\", \"appTo\":\"1\", \"R\":2, \
        \"G\":3, \"B\":4, \"I\":5}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessRgbiPost(Payload, &output);

    ColorMsg_t *pColor = (ColorMsg_t *)LastSetMsg.pStream;
    CHECK((pColor->red == 2));
    CHECK((pColor->green == 3));
    CHECK((pColor->blue == 4));
    CHECK((pColor->intensity == 5));
    CHECK((LastSetMsg.Apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/RGBWAsync" */
TEST_CASE("Post RGBWAsync Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbwAsync\", \"appTo\":\"1\", \"R\":2, \
        \"G\":3, \"B\":4, \"W\":6}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessRgbwPost(Payload, &output);
    ColorMsg_t *pColor = (ColorMsg_t *)LastSetMsg.pStream;
    CHECK((pColor->red == 2));
    CHECK((pColor->green == 3));
    CHECK((pColor->blue == 4));
    CHECK((pColor->white == 6));
    CHECK((LastSetMsg.Apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/RGBWSingle" */
TEST_CASE("Post RGBWSingle Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbwStrip\", \"appTo\":\"1\", \"R\":2, \
        \"G\":3, \"B\":4, \"W\":6}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessRgbwSinglePost(Payload, &output);
    ColorMsg_t *pColor = (ColorMsg_t *)LastSetMsg.pStream;
    CHECK((pColor->red == 2));
    CHECK((pColor->green == 3));
    CHECK((pColor->blue == 4));
    CHECK((pColor->white == 6));
    CHECK((LastSetMsg.Apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/IValues" */
TEST_CASE("Post IValues Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"grayPort\", \"appTo\":\"1\", \
                \"G1\":1,\"G2\":2,\"G3\":3,\"G4\":4,\
                \"G5\":5,\"G6\":6,\"G7\":7,\"G8\":8, \
                \"G9\":9,\"G10\":10,\"G11\":11,\"G12\":12,\
                \"G13\":13,\"G14\":14,\"G15\":15,\"G16\":16}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessGrayValuesPost(Payload, &output);
    GrayValMsg_t *pValues = (GrayValMsg_t *)LastSetMsg.pStream;

    for (size_t i = 0; i < 16; i++) {
        CHECK((pValues->gray[i] == 1 + i));
    }
    CHECK((LastSetMsg.Apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetDevice/WiFiConnect" */
TEST_CASE("Post SetWiFiConnect Handler-Tests", "[ConfigPost]") {
    const char *Payload = "{\"ssid\":\"abc\",\"password\":\"123\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessWiFiStatusSet(Payload, &output);
}

/** "POST /api/SetDevice/ResetWiFiConnect" */
TEST_CASE("Post ResetWiFiConnect Handler-Tests", "[ConfigPost]") {
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);
    bool result = ProcessWiFiStatusGet(nullptr, &output);
}


/** "POST /api/SetPort/genericRGB" */
TEST_CASE("Post genericRGB Values Handler-Tests", "[ColorPost]") {
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr, &global_config);

    SECTION("Effect Target") {
        const char *Payload =
            "{\"form\":\"genericForm\", \"target\":\"effectCh\", \"type\":\"RGB\", \"appTo\":\"1\", \"R\":2, \
        \"G\":3, \"B\":4, \"I\":5}";
        const char *output;
        bool result = ProcessGenericRgbPost(Payload, &output);

        ColorMsg_t *pColor = (ColorMsg_t *)LastSetMsg.pStream;
        CHECK((pColor->red == 2));
        CHECK((pColor->green == 3));
        CHECK((pColor->blue == 4));
        CHECK((LastSetMsg.Apply.ApplyTo[0] == 0x00000001));
    }
}

