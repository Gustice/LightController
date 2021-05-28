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

deviceConfig_t config{.StartUpMode = DeviceStartMode::RunDemo,
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
    }};

TEST_CASE("Instantiate Driver object without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &config);
    REQUIRE(true);
}

TEST_CASE("Demo can be triggered without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &config);
    dut.DemoTick();
}


deviceConfig_t startupConfig{.StartUpMode = DeviceStartMode::StartImage,
    .SyncLeds{
        .IsActive = true,
        .Strip{
            .LedCount = 4,
        },
        .Color{.red = 0xFF, .green = 0, .blue = 0, .white = 0},
        .Delay = 1,
    },
    .AsyncLeds{
        .IsActive = true,
        .Strip{.LedCount = 6},
        .Color{.red = 0, .green = 0xFF, .blue = 0, .white = 0},
        .Delay = 2,
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
    },
    .EffectMachines{
        {.Target = TargetGate::SyncLed, .ApplyFlags = 0x03, .Color = {255, 0, 0, 0}, .Delay = 0},
        {.Target = TargetGate::AsyncLed, .ApplyFlags = 0x01, .Color = {0, 255, 0, 0}, .Delay = 1},
        {.Target = TargetGate::LedStrip, .ApplyFlags = 0x01, .Color = {0, 0, 255, 0}, .Delay = 2},
        {.Target = TargetGate::SyncLed, .ApplyFlags = 0x04, .Color = {0, 255, 0, 0}, .Delay = 3},
    }};

TEST_CASE("Startup can be triggered without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &startupConfig);
    for (size_t i = 0; i < 64; i++) {
        dut.EffectTick();
    }
    for (size_t i = 0; i < 64; i++) {
        dut.EffectTick();
    }
}