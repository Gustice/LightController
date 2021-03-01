#include "ChannelIndexes.h"
#include "TestUtils.h"

Color_t color1{1, 11, 21, 31};
Color_t color2{2, 0, 0, 0};
Color_t color3{3, 0, 0, 0};
Color_t color4{4, 0, 0, 0};
Color_t color5{5, 0, 0, 0};
Color_t color6{6, 0, 0, 0};
Color_t color7{7, 0, 0, 0};
Color_t color8{8, 0, 0, 0};

const size_t cCount = 8;
const Color_t *testColors[cCount]{
    &color1,
    &color2,
    &color3,
    &color4,
    &color5,
    &color6,
    &color7,
    &color8,
};

TEST_CASE("Instantiate Channel-object without any trouble", "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);
    REQUIRE(true);
}

TEST_CASE("All Variables are initialized correctly", "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);

    CHECK(dut.ImageSize == sizeof(Color_t) * count);
    uint8_t compStream[dut.ImageSize];
    memset(compStream, 0, dut.ImageSize);

    CHECK(memcmp(dut.Image, compStream, dut.ImageSize) == 0);

    CHECK(dut.Led.GetIndex() == 0);
    CHECK(dut.Color.GetIndex() == 0);
}

TEST_CASE("Clear Image clears buffer to zero", "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);

    dut.Image[0].red = 0x12;
    dut.Image[1].green = 0x13;
    dut.Image[2].blue = 0x14;
    dut.Image[3].white = 0x15;
    dut.ClearImage();

    uint8_t compStream[dut.ImageSize];
    memset(compStream, 0, dut.ImageSize);

    CHECK(memcmp(dut.Image, compStream, dut.ImageSize) == 0);
}

TEST_CASE("FirstColor is completely set. After first inkrement", "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 1);
    CHECK(dut.Image[0].green == 11);
    CHECK(dut.Image[0].blue == 21);
    CHECK(dut.Image[0].white == 31);
}

TEST_CASE("FirstColor moves once though chain. After that it switches to second color",
    "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 1);
    CHECK(dut.Image[1].red == 0);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 0);
    CHECK(dut.Image[1].red == 1);
    CHECK(dut.Image[2].red == 0);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[1].red == 0);
    CHECK(dut.Image[2].red == 1);
    CHECK(dut.Image[3].red == 0);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[2].red == 0);
    CHECK(dut.Image[3].red == 1);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 2);
    CHECK(dut.Image[1].red == 0);
    CHECK(dut.Image[3].red == 0);
}

TEST_CASE("Color rotates correctly back after all Colors have been used", "[ChannelIndex]") {
    size_t count = 4;
    ChannelIndexes dut(count, testColors, cCount);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 1);
    CHECK(dut.Image[1].red == 0);

    for (size_t i = 0; i < 4; i++) {
        dut.ClearImage();
        dut.SetNextSlotMindOverflow();
    }
    CHECK(dut.Image[0].red == 2);
    CHECK(dut.Image[1].red == 0);

    for (size_t i = 0; i < 4; i++) {
        dut.ClearImage();
        dut.SetNextSlotMindOverflow();
    }
    CHECK(dut.Image[0].red == 3);
    CHECK(dut.Image[1].red == 0);

    for (size_t i = 0; i < 4*6-1; i++) {
        dut.ClearImage();
        dut.SetNextSlotMindOverflow();
    }
    CHECK(dut.Image[2].red == 0);
    CHECK(dut.Image[3].red == 8);

    dut.ClearImage();
    dut.SetNextSlotMindOverflow();
    CHECK(dut.Image[0].red == 1);
    CHECK(dut.Image[1].red == 0);
}