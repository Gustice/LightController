#include "ProcessHandler.h"
#include "TestUtils.h"

extern ColorMsg_t LastColorMsg;
extern GrayValMsg_t LastGrayValMsg;

TEST_CASE("Evaluate String Functions again (problems with compiler)", "[Basics]") {
    CHECK(2 == std::stoi("2"));
    CHECK(3 == std::stoi(" 3"));
    CHECK(4 == std::stoi("4 "));

    CHECK(2 == std::stoi("2.0"));
    CHECK(3 == std::stoi(" 3  . 1"));
    CHECK(4 == std::stoi("4 .3   "));
}


/** "POST /api/SetPort/RGBISync" */
TEST_CASE("Post RGBISync Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbiSync\", \"appTo\":\"1\", \"R\":\"2\", \
        \"G\":\"3\", \"B\":\"4\", \"I\":\"5\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbiPost(Payload, &output);
    CHECK((LastColorMsg.red == 2));
    CHECK((LastColorMsg.green == 3));
    CHECK((LastColorMsg.blue == 4));
    CHECK((LastColorMsg.intensity == 5));
    CHECK((LastColorMsg.apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/RGBWAsync" */
TEST_CASE("Post RGBWAsync Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbwAsync\", \"appTo\":\"1\", \"R\":\"2\", \
        \"G\":\"3\", \"B\":\"4\", \"W\":\"6\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbwPost(Payload, &output);
    CHECK((LastColorMsg.red == 2));
    CHECK((LastColorMsg.green == 3));
    CHECK((LastColorMsg.blue == 4));
    CHECK((LastColorMsg.white == 6));
    CHECK((LastColorMsg.apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/RGBWSingle" */
TEST_CASE("Post RGBWSingle Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"rgbwStrip\", \"appTo\":\"1\", \"R\":\"2\", \
        \"G\":\"3\", \"B\":\"4\", \"W\":\"6\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbwSinglePost(Payload, &output);
    CHECK((LastColorMsg.red == 2));
    CHECK((LastColorMsg.green == 3));
    CHECK((LastColorMsg.blue == 4));
    CHECK((LastColorMsg.white == 6));
    CHECK((LastColorMsg.apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SetPort/IValues" */
TEST_CASE("Post IValues Values Handler-Tests", "[ColorPost]") {
    const char *Payload = "{\"form\":\"greyPort\", \"appTo\":\"1\", \
                \"G1\":\"1\",\"G2\":\"2\",\"G3\":\"3\",\"G4\":\"4\",\
                \"G5\":\"5\",\"G6\":\"6\",\"G7\":\"7\",\"G8\":\"8\", \
                \"G9\":\"9\",\"G10\":\"10\",\"G11\":\"11\",\"G12\":\"12\",\
                \"G13\":\"13\",\"G14\":\"14\",\"G15\":\"15\",\"G16\":\"16\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessGrayValuesPost(Payload, &output);
    for (size_t i = 0; i < 16; i++) {
        CHECK((LastGrayValMsg.gray[i] == 1+i));
    }
    CHECK((LastColorMsg.apply.ApplyTo[0] == 0x00000001));
}

/** "POST /api/SaveToPage" */
TEST_CASE("Post SetPage Handler-Tests", "[ConfigPost]") {
    const char *Payload = "{\"Page\":1}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessSaveToPage(Payload, &output);
}
/** "POST /api/ResetProgram" */
TEST_CASE("Post ResetPages Handler-Tests", "[ConfigPost]") {
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessResetPages(nullptr, &output);
}

/** "POST /api/SetDevice/WiFiConnect" */
TEST_CASE("Post SetWiFiConnect Handler-Tests", "[ConfigPost]") {
    const char *Payload = "{\"ssid\":\"abc\",\"password\":\"123\"}";
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessWiFiStatusSet(Payload, &output);
}

/** "POST /api/SetDevice/ResetWiFiConnect" */
TEST_CASE("Post ResetWiFiConnect Handler-Tests", "[ConfigPost]") {
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessWiFiStatusGet(nullptr, &output);
}