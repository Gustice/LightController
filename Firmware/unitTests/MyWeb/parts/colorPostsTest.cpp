#include "testInclude.h"

TEST_CASE("Construction of ColorPost object just for fun", "[colorPost]") { RgbPost dut(1); }

static const Color_t _colorBlack = {0, 0, 0, 0};

bool ColorCompare(Color_t c1, Color_t c2) {
    if (c1.red != c2.red)
        return false;
    if (c1.green != c2.green)
        return false;
    if (c1.blue != c2.blue)
        return false;
    if (c1.white != c2.white)
        return false;

    return true;
}

TEST_CASE("Test if constructed object is initiated correctly", "[colorPost]") {
    { // Get sure that the available memory is preset with some junk data
        Color_t memfill[25];
        memset(memfill, 0xc0fe, sizeof(Color_t) * 25);
    }
    {
        RgbPost dut(10);
        const Color_t *c = dut.GetColors();
        for (size_t i = 0; i < 10; i++) {
            CHECK(ColorCompare(c[i], _colorBlack));
        }
    }
}

TEST_CASE("Different attempts of setting one color in object containing on slot",
          "[colorPost, single]") {
    RgbPost dut(1);
    Color_t c = {1, 2, 3, 4};

    SECTION("Setting of all LEDs with x") {
        dut.EvalPost(c, 1, 5, "x");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(c, *dR));
    }
    SECTION("Setting of all LEDs with X") {
        dut.EvalPost(c, 1, 5, "X");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(c, *dR));
    }
    SECTION("Setting nothing with n -> Error / no effect") {
        dut.EvalPost(c, 1, 5, "n");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(*dR, _colorBlack));
    }
    SECTION("Setting nothing empty '' -> Error / no effect") {
        dut.EvalPost(c, 1, 5, "");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(*dR, _colorBlack));
    }
    SECTION("Setting first LED with 1") {
        dut.EvalPost(c, 1, 5, "1");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(c, *dR));
    }
    SECTION("Setting first LED with 1x -> Should be tolerated") {
        dut.EvalPost(c, 1, 5, "1x");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(c, *dR));
    }
    SECTION("Setting second LED with 2 -> should be ignored") {
        dut.EvalPost(c, 1, 5, "2");
        const Color_t *dR = dut.GetColors();

        CHECK(ColorCompare(*dR, _colorBlack));
    }
}

TEST_CASE("Different attempts of setting one color in object containing three slots",
          "[colorPost, multi]") {
    uint16_t size = 3;
    RgbPost dut(size);
    Color_t c = {11, 12, 13, 14};

    SECTION("Setting of all LEDs with x") {
        dut.EvalPost(c, size, 5, "x");
        const Color_t *dR = dut.GetColors();

        for (size_t i = 0; i < size; i++) {
            INFO("Step " << i)
            CHECK(ColorCompare(c, dR[i]));
        }
    }
    SECTION("Setting first LED with 1") {
        dut.EvalPost(c, size, 5, "1");
        const Color_t *dR = dut.GetColors();

        for (size_t i = 0; i < size; i++) {
            INFO("Step " << i)
            if (i == 0)
                CHECK(ColorCompare(c, dR[i]));
            else
                CHECK(ColorCompare(dR[i], _colorBlack));
        }
    }
    SECTION("Setting last LED with 3") {
        dut.EvalPost(c, size, 5, "3");
        const Color_t *dR = dut.GetColors();

        for (size_t i = 2; i < size; i++) {
            INFO("Step " << i)
            if (i == 2)
                CHECK(ColorCompare(c, dR[i]));
            else
                CHECK(ColorCompare(dR[i], _colorBlack));
        }
    }
    // SECTION("Setting second LED with 2 -> should be ignored") {
    //     dut.EvalPost(c, size, 5, "2");
    //     const Color_t *dR = dut.GetColors();

    //     CHECK(ColorCompare(*dR, _colorBlack));
    // }
}
// Input looks somewhat like that
// channel=ch1sync&stripLen=1&applyTo=1&redV=160&greenV=0&blueV=0&intensV=0