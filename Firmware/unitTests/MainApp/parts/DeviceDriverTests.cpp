#include "DeviceDriver.h"
#include "TestUtils.h"

Apa102 sync(8);
Ws2812 async(8);
RgbwStrip strip(1);
Pca9685 expander(16);

static Color_t color1{1, 11, 21, 31};
static Color_t color2{2, 0, 0, 0};
static Color_t color3{3, 0, 0, 0};
static Color_t color4{4, 0, 0, 0};
static Color_t color5{5, 0, 0, 0};
static Color_t color6{6, 0, 0, 0};
static Color_t color7{7, 0, 0, 0};
static Color_t color8{8, 0, 0, 0};

static const size_t cCount = 8;
static const Color_t *testColors[cCount]{
    &color1,
    &color2,
    &color3,
    &color4,
    &color5,
    &color6,
    &color7,
    &color8,
};

deviceConfig_t config{.StartUpMode = DeviceStartMode_t::RunDemo,
    .SyncLeds{.IsActive = true,
        .Strip{
            .LedCount = 4,
        }},
    .AsyncLeds{
        .IsActive = true,
        .Strip{.LedCount = 6},
    },
    .RgbStrip{
        .IsActive = true,
        .Strip{
            .LedCount = 6,
        },
        .ChannelCount = 2,
    },
    .I2cExpander{
        .IsActive = true,
        .GrayValues = 8,
    }};

TEST_CASE("Instantiate object without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &config);
    REQUIRE(true);
}

// TEST_CASE("All Variables are initialized correctly", "[ChannelIndex]") {
//     DeviceDriver dut(&sync, &async, &strip, &expander, &config);
// }