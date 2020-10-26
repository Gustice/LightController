#include "TestUtils.h"
#include "Ws2812.h"

static const size_t LedWordSize = 3;

TEST_CASE("Construction of async LED-strip object as precondition", "[ws2812]") {
    size_t LedCount = 1;
    RmtPort rmt;
    Ws2812 dut(&rmt, LedCount);
    REQUIRE (true);
}

static void PrettyPrintSentData(uint8_t *txData, size_t len, size_t ledCnt);

static Color_t black = {0, 0, 0, 0};
TEST_CASE("Sending of one empty async LED-Frame", "[ws2812]") {
    size_t LedCount = 1;
    RmtPort rmt;
    Ws2812 dut(&rmt, LedCount);
    uint8_t compare[] = {0, 0, 0};

    dut.SendImage(&black);

    REQUIRE(rmt.LastLen == sizeof(compare));
    PrettyPrintSentData(rmt.SendBuffer, rmt.LastLen, LedCount);
    CHECK(memcmp(compare, rmt.SendBuffer, rmt.LastLen) == 0);
}

static Color_t arbitrary = {1, 2, 3, 4};
TEST_CASE("Sending of one async LED-Frame with unique Values", "[ws2812]") {
    size_t LedCount = 1;
    RmtPort rmt;
    Ws2812 dut(&rmt, LedCount);
    uint8_t compare[] = {2, 1, 3};

    dut.SendImage(&arbitrary);

    REQUIRE(rmt.LastLen == sizeof(compare));
    PrettyPrintSentData(rmt.SendBuffer, rmt.LastLen, LedCount);
    CHECK(memcmp(compare, rmt.SendBuffer, rmt.LastLen) == 0);
}

static Color_t arbitrary2[2] = {{1, 2, 3, 4}, {5, 6, 7, 8}};
TEST_CASE("Sending of two async LED-Frames with unique Values", "[ws2812]") {
    size_t LedCount = 2;
    RmtPort rmt;
    Ws2812 dut(&rmt, LedCount);
    uint8_t compare[] = {2, 1, 3, 6, 5, 7};

    dut.SendImage(arbitrary2);

    REQUIRE(rmt.LastLen == sizeof(compare));
    PrettyPrintSentData(rmt.SendBuffer, rmt.LastLen, LedCount);
    CHECK(memcmp(compare, rmt.SendBuffer, rmt.LastLen) == 0);
}

typedef struct Ws2812Color_def {
    uint8_t Green;
    uint8_t Red;
    uint8_t Blue;
} ws2812Color_t;

static void PrettyPrintSentData(uint8_t *txData, size_t len, size_t ledCnt) {
    char buffer[256];
    for (int i = 0; i < ledCnt; i++) {
        sprintf(buffer, "    LED %4d data: %3d_B  %3d_G  %3d_R", i, buffer[i * 3 + 2],
                buffer[i * 3], buffer[i * 3 + 1]);
        UNSCOPED_INFO(buffer);
    }
}