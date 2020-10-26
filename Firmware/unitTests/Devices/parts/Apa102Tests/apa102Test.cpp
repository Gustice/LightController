#include "Apa102.h"
#include "TestUtils.h"

static const size_t LedWordSize = 4;
static const uint32_t StartFrame = 0x00000000;
static const uint8_t StartSign = 0xC0;
static const uint32_t EndFrame = 0xC0000000;

TEST_CASE("Construction of LED-strip object as precondition", "[apa102]") {
    size_t LedCount = 1;
    SpiPort spi;
    Apa102 dut(&spi, LedCount);
    REQUIRE (true);
}

static void PrettyPrintSentData(uint8_t *txData, size_t len, size_t ledCnt);

static Color_t black = {0, 0, 0, 0};
TEST_CASE("Sending of one empty LED-Frame", "[apa102]") {
    size_t LedCount = 1;
    SpiPort spi;
    Apa102 dut(&spi, LedCount);
    uint8_t compare[] = {0, 0, 0, 0, StartSign + 1, 0, 0, 0, StartSign, 0, 0, 0};

    dut.SendImage(&black);

    REQUIRE(spi.LastLen == 3 * LedWordSize);
    PrettyPrintSentData(spi.SendBuffer, spi.LastLen, LedCount);
    CHECK(memcmp(compare, spi.SendBuffer, spi.LastLen) == 0);
}

static Color_t arbitrary = {1, 2, 3, 4};
TEST_CASE("Sending of one LED-Frame with unique Values", "[apa102]") {
    size_t LedCount = 1;
    SpiPort spi;
    Apa102 dut(&spi, LedCount);
    uint8_t compare[] = {0, 0, 0, 0, StartSign + 1, 3, 2, 1, StartSign, 0, 0, 0};

    dut.SendImage(&arbitrary);

    REQUIRE(spi.LastLen == 3 * LedWordSize);
    PrettyPrintSentData(spi.SendBuffer, spi.LastLen, LedCount);
    CHECK(memcmp(compare, spi.SendBuffer, spi.LastLen) == 0);
}

TEST_CASE("Sending of one LED-Frame with unique Values and brightness", "[apa102]") {
    size_t LedCount = 1;
    SpiPort spi;
    Apa102 dut(&spi, LedCount);
    uint8_t compare[] = {0, 0, 0, 0, StartSign | 0x10, 3, 2, 1, StartSign, 0, 0, 0};

    dut.SetBrightness(0x80);
    dut.SendImage(&arbitrary);

    REQUIRE(spi.LastLen == 3 * LedWordSize);
    PrettyPrintSentData(spi.SendBuffer, spi.LastLen, LedCount);
    CHECK(memcmp(compare, spi.SendBuffer, spi.LastLen) == 0);
}

static Color_t arbitrary2[2] = {{1, 2, 3, 4}, {5, 6, 7, 8}};
TEST_CASE("Sending of two LED-Frames with unique Values", "[apa102]") {
    size_t LedCount = 2;
    SpiPort spi;
    Apa102 dut(&spi, LedCount);
    uint8_t compare[] = {0,         0, 0, 0, StartSign + 1, 3, 2, 1, StartSign + 1, 7, 6, 5,
                         StartSign, 0, 0, 0};

    dut.SendImage(arbitrary2);

    REQUIRE(spi.LastLen == (1 + 2 + 1) * LedWordSize);
    PrettyPrintSentData(spi.SendBuffer, spi.LastLen, LedCount);
    CHECK(memcmp(compare, spi.SendBuffer, spi.LastLen) == 0);

    dut.SetBrightness(0x88);
    dut.SendImage(arbitrary2);

    compare[4] |= 0x10;
    compare[8] |= 0x10;
    PrettyPrintSentData(spi.SendBuffer, spi.LastLen, LedCount);
    CHECK(memcmp(compare, spi.SendBuffer, spi.LastLen) == 0);
}

static Color_t ColorFrame[65];
TEST_CASE("Trying different LED strip lengths and checking length of EndFrame", "[apa102]") {

    SECTION("LEDs are more than covered with on EndFrame") {
        size_t LedCount = 63;
        SpiPort spi;
        Apa102 dut(&spi, LedCount);
        dut.SendImage(ColorFrame);

        CHECK(spi.LastLen == (1 + LedCount + 1) * LedWordSize);
    }
    SECTION("LEDs are covered with on EndFrame") {
        size_t LedCount = 64;
        SpiPort spi;
        Apa102 dut(&spi, LedCount);
        dut.SendImage(ColorFrame);

        CHECK(spi.LastLen == (1 + LedCount + 1) * LedWordSize);
    }
    SECTION("LEDs are covered with two EndFrame") {
        size_t LedCount = 65;
        SpiPort spi;
        Apa102 dut(&spi, LedCount);
        dut.SendImage(ColorFrame);

        CHECK(spi.LastLen == (1 + LedCount + 2) * LedWordSize);
    }
}

typedef struct Apa102Color_def {
    uint8_t Bright;
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
} Apa102Color_t;

static void PrettyPrintSentData(uint8_t *txData, size_t len, size_t ledCnt) {
    char buffer[256];

    uint32_t *data = (uint32_t *)txData;
    sprintf(buffer, "    Startcode 0x%x", data[0]);
    UNSCOPED_INFO(buffer);
    int i = 1;
    for (; i <= ledCnt; i++) {
        Apa102Color_t *word = (Apa102Color_t *)&data[i];
        sprintf(buffer, "    LED %4d data: 0x%2x_C  %2d_I %3d_B  %3d_G  %3d_R", i,
                word->Bright & 0xC0, word->Bright & 0x1F, word->Blue, word->Green, word->Red);
        UNSCOPED_INFO(buffer);
    }
    for (; i < len / 4; i++) {
        sprintf(buffer, "    EndCode 0x%x", data[i]);
        UNSCOPED_INFO(buffer);
    }
}