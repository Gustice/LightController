#include "TestUtils.h"
#include "ProcessHandler.h"

extern ColorMsg_t LastMessage;
bool StringIsEqual(const char * excpected, const char * actual);

TEST_CASE("Evaluate String Functions again (problems with compiler)", "[Basics]") {
    CHECK(2 == std::stoi("2"));
    CHECK(3 == std::stoi(" 3"));
    CHECK(4 == std::stoi("4 "));

    CHECK(2 == std::stoi("2.0"));
    CHECK(3 == std::stoi(" 3  . 1"));
    CHECK(4 == std::stoi("4 .3   "));
}


/** "POST /api/SetPort/RGBISync" */
TEST_CASE("Post RGBISync Values Handler-Tests", "[RgbiPost]") {
    const char *Payload = "{\"form\":\"rgbiSync\", \"appTo\":\"1\", \"R\":\"2\", \"G\":\"3\", \"B\":\"4\", \"I\":\"5\"}";
    const char * output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbiPost(Payload, &output);
    CHECK( LastMessage.red == 2);
    CHECK( LastMessage.green == 3);
    CHECK( LastMessage.blue == 4);
    CHECK( LastMessage.intensity == 5);
    CHECK( LastMessage.apply.ApplyTo[0] == 0x00000001 );
}

/** "POST /api/SetPort/RGBWAsync" */
TEST_CASE("Post RGBWAsync Values Handler-Tests", "[RgbwPost]") {
    const char *Payload = "{\"form\":\"rgbwAsync\", \"appTo\":\"1\", \"R\":\"2\", \"G\":\"3\", \"B\":\"4\", \"W\":\"6\"}";
    const char * output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbwPost(Payload, &output);
    CHECK( LastMessage.red == 2);
    CHECK( LastMessage.green == 3);
    CHECK( LastMessage.blue == 4);
    CHECK( LastMessage.white == 6);
    CHECK( LastMessage.apply.ApplyTo[0] == 0x00000001 );
}

/** "POST /api/SetPort/RGBWSingle" */
TEST_CASE("Post RGBWSingle Values Handler-Tests", "[RgbwSinglePost]") {
    const char *Payload = "{\"form\":\"rgbwStrip\", \"appTo\":\"1\", \"R\":\"2\", \"G\":\"3\", \"B\":\"4\", \"W\":\"6\"}";
    const char * output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
    bool result = ProcessRgbwSinglePost(Payload, &output);
    CHECK( LastMessage.red == 2);
    CHECK( LastMessage.green == 3);
    CHECK( LastMessage.blue == 4);
    CHECK( LastMessage.intensity ==6);
    CHECK( LastMessage.apply.ApplyTo[0] == 0x00000001 );
}


// /** "POST /api/SetPort/IValues" */
// TEST_CASE("Post IValues Values Handler-Tests", "[IValPost]") {
//     const char *Payload = "{\"form\":\"greyPort\",\"G1\":\"0\",\"G2\":\"0\",\"G3\":\"0\",\"G4\":\"0\",\"G5\":\"0\",\"G6\":\"0\",\"G7\":\"0\",\"G8\":\"0\",\"G9\":\"0\",\"G10\":\"0\",\"G11\":\"0\",\"G12\":\"0\",\"G13\":\"0\",\"G14\":\"0\",\"G15\":\"0\",\"G16\":\"0\"}";
//     const char * output;
//     SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr);
//     bool result = ProcessGrayValuesPost(Payload, &output);
//     CHECK( LastMessage.red == 1);
//     CHECK( LastMessage.green == 2);
//     CHECK( LastMessage.blue == 3);
//     CHECK( LastMessage.intensity == 4);
//     CHECK( LastMessage.apply.ApplyTo[0] == 0x00000001 );
// }

/** "POST /api/SaveToPage" */
/** "POST /api/ResetProgram" */
/** "POST /api/SetDevice/WiFiConnect" */
/** "POST /api/SetDevice/ResetWiFiConnect" */


bool StringIsEqual(const char * excpected, const char * actual)
{
    return true;
}