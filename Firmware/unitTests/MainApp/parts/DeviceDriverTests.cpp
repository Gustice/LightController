#include "DeviceDriver.h"
#include "TestUtils.h"

Apa102 sync(4);
Ws2812 async(6);
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

deviceConfig_t config{
    .StartUpMode = DeviceStartMode_t::RunDemo,
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
        .ChannelCount = 1,
    },
    .I2cExpander{
        .IsActive = true,
        .Device{
            .LedCount = 8,
        },
    }
};

TEST_CASE("Instantiate Driver object without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &config);
    REQUIRE(true);
}

TEST_CASE("All Driver Variables are initialized correctly", "[ChannelIndex]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &config);

    ColorMsg_t col1 {
        .channel = RgbChannel::RgbiSync,
        .red = 1,
        .apply {
            .ApplyTo{ 0xFFFF, 0,0,0,0, }
        }
    };
    dut.ApplyRgbColorMessage(&col1);

    CHECK(sync.buffer[0].red == 1);
    CHECK(sync.buffer[1].red == 1);
    CHECK(sync.buffer[2].red == 1);
    CHECK(sync.buffer[3].red == 1);
    CHECK(sync.buffer[4].red == 0);
 
    ColorMsg_t col2 {
        .channel = RgbChannel::RgbwAsync,
        .green = 1,
        .apply {
            .ApplyTo{ 0xFFFF, 0,0,0,0, }
        }
    };
    dut.ApplyRgbColorMessage(&col2);

    CHECK(async.buffer[0].green == 1);
    CHECK(async.buffer[1].green == 1);
    CHECK(async.buffer[2].green == 1);
    CHECK(async.buffer[3].green == 1);
    CHECK(async.buffer[4].green == 1);
    CHECK(async.buffer[5].green == 1);
    CHECK(async.buffer[6].green == 0);
    
    ColorMsg_t col3 {
        .channel = RgbChannel::RgbwPwm,
        .blue = 1,
        .apply {
            .ApplyTo{ 0xFFFF, 0,0,0,0, }
        }
    };
    dut.ApplyRgbColorMessage(&col3);

    CHECK(strip.buffer[0].blue == 1);
    CHECK(strip.buffer[1].blue == 0);
}