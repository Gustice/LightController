#include "TestUtils.h"
#include "RgbwStrip.h"

TEST_CASE("Construction of RGB-LED-strip object as precondition", "[rgbStrip]") {
    PwmPort r, g, b, w;
    RgbwStrip dut(&r, &g, &b, &w);
    REQUIRE (true);
}

static void PrettyPrintSentData(uint8_t *txData, size_t len, size_t ledCnt);


static Color_t black = {0, 0, 0, 0};
TEST_CASE("Sending of one empty RGB-LED-Frame", "[rgbStrip]") {
    PwmPort r, g, b, w;
    RgbwStrip dut(&r, &g, &b, &w);

    dut.SetImage(&black);

    REQUIRE(r.Value == 0);
    REQUIRE(g.Value == 0);
    REQUIRE(b.Value == 0);
    REQUIRE(w.Value == 0);
}

static Color_t arbitrary = {1, 2, 3, 4};
TEST_CASE("Sending of one RGB-LED-Frame with unique Values", "[rgbStrip]") {
    PwmPort r, g, b, w;
    RgbwStrip dut(&r, &g, &b, &w);
    uint8_t compare[] = {2, 1, 3};

    dut.SetImage(&arbitrary);

    REQUIRE(r.Value == 1);
    REQUIRE(g.Value == 2);
    REQUIRE(b.Value == 3);
    REQUIRE(w.Value == 4);
}

TEST_CASE("Setup of Strip without white-channel yields no Problem", "[rgbStrip]") {
    PwmPort r, g, b;
    RgbwStrip dut(&r, &g, &b);

    dut.SetImage(&arbitrary);

    REQUIRE(r.Value == 1);
    REQUIRE(g.Value == 2);
    REQUIRE(b.Value == 3);
}
