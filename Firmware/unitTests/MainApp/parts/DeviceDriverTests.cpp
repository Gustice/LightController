#include "DeviceDriver.h"
#include "TestUtils.h"

Apa102 sync(4);
Ws2812 async(6);
RgbwStrip strip(1);
Pca9685 expander(16);

effectProcessor_t prc[] = {
    {.Target = TargetGate::SyncLed, .ApplyFlags = 0x03, .Color = {255, 0, 0, 0}, .Delay = 0},
    {.Target = TargetGate::AsyncLed, .ApplyFlags = 0x01, .Color = {0, 255, 0, 0}, .Delay = 1},
    {.Target = TargetGate::LedStrip, .ApplyFlags = 0x01, .Color = {0, 0, 255, 0}, .Delay = 2},
    {.Target = TargetGate::SyncLed, .ApplyFlags = 0x04, .Color = {0, 255, 0, 0}, .Delay = 3},
};
EffectComplex effCmplx(4, prc);

static Color_t dark{0, 0, 0, 0};
static Color_t *pDark = &dark;
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
    DeviceDriver dut(&sync, &async, &strip, &expander, &effCmplx, &config);
    REQUIRE(true);
}

TEST_CASE("Demo can be triggered without any trouble", "[DeviceDriver]") {
    DeviceDriver dut(&sync, &async, &strip, &expander, &effCmplx, &config);
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
    DeviceDriver dut(&sync, &async, &strip, &expander, &effCmplx, &startupConfig);
    for (size_t i = 0; i < 64; i++) {
        dut.EffectTick();
    }
    for (size_t i = 0; i < 64; i++) {
        dut.EffectTick();
    }
}


bool ColorIsEqual(Color_t *c1, Color_t *c2) {
    if (c1->red != c2->red)
        return false;
    if (c1->green != c2->green)
        return false;
    if (c1->blue != c2->blue)
        return false;
    if (c1->white != c2->white)
        return false;

    return true;
}


TEST_CASE("Set Values works for all cases", "[DeviceDriver]") {
    EffectComplex eff(4, startupConfig.EffectMachines);
    DeviceDriver dut(&sync, &async, &strip, &expander, &eff, &startupConfig);
    ReqColorIdx_t idx = {.type = RgbChannel::None_Error, .chIdx = 0, .portIdx = 0};
    CHECK(ColorIsEqual(&sync.buffer[0], pDark));
    CHECK(ColorIsEqual(&async.buffer[0], pDark));
    CHECK(ColorIsEqual(&strip.buffer[0], pDark));

    ColorMsg_t msg = {.red = 1, .green = 2, .blue = 3, .white = 4, .intensity = 5};
    Color_t refColor = {.red = 1, .green = 2, .blue = 3, .white = 4};
    ApplyIndexes_t app = {
        .Items = 1,
        .FirstTarget{.type = RgbChannel::RgbiSync, .chIdx = 0, .portIdx = 0},
        .ApplyTo = {0x00000001, 0, 0, 0, 0},
    };

    SECTION("Setting Sync RGB-Channels") {
        idx.type = RgbChannel::RgbiSync;
        dut.SetValue(idx, (uint8_t *)&msg, sizeof(ColorMsg_t), &app);
        CHECK(ColorIsEqual(&sync.buffer[0], &refColor));
        CHECK(ColorIsEqual(&sync.buffer[1], pDark));
    }
    SECTION("Setting Async RGB-Channels") {
        idx.type = RgbChannel::RgbwAsync;
        dut.SetValue(idx, (uint8_t *)&msg, sizeof(ColorMsg_t), &app);
        CHECK(ColorIsEqual(&async.buffer[0], &refColor));
        CHECK(ColorIsEqual(&async.buffer[1], pDark));
    }
    SECTION("Setting PWM RGB-Strips") {
        idx.type = RgbChannel::RgbwPwm;
        dut.SetValue(idx, (uint8_t *)&msg, sizeof(ColorMsg_t), &app);
        CHECK(ColorIsEqual(&strip.buffer[0], &refColor));
        CHECK(ColorIsEqual(&strip.buffer[1], pDark));
    }
    // SECTION("Setting I2C-Expander") {}
    SECTION("Setting Effect channels") {
        idx.type = RgbChannel::EffectProcessor;
        dut.SetValue(idx, (uint8_t *)&msg, sizeof(ColorMsg_t), &app);
        Color_t retC;
        eff.ReadColor(0, retC);
        CHECK(ColorIsEqual(&retC, &refColor));
    }
}

const ApplyIndexes_t apply2First = {
        .Items = 1,
        .FirstTarget{.type = RgbChannel::RgbiSync, .chIdx = 0, .portIdx = 0},
        .ApplyTo = {0x00000001, 0, 0, 0, 0},
    };


TEST_CASE("Get Values works for all cases", "[DeviceDriver]") {
    EffectComplex eff(4, startupConfig.EffectMachines);
    DeviceDriver dut(&sync, &async, &strip, &expander, &eff, &startupConfig);
    ReqColorIdx_t idx = {.type = RgbChannel::None_Error, .chIdx = 0, .portIdx = 0};

    Color_t * refColor = nullptr;
    Color_t refColor1 = {1,2,3,4};
    Color_t refColor2 = {11,12,13,14};
    Color_t refColor3 = {21,22,23,24};
    Color_t refColor4 = {31,32,33,34};

    SetColor(dut.Images[RgbChannel::RgbiSync]->Image, &refColor1);
    SetColor(dut.Images[RgbChannel::RgbwAsync]->Image, &refColor2);
    SetColor(dut.Images[RgbChannel::RgbwPwm]->Image, &refColor3);
    eff.FadeToColor(0, refColor4);

    ColorMsg_t msg;
    SECTION("Getting Sync RGB-Channels") {
        idx.type = RgbChannel::RgbiSync;
        refColor = &refColor1;
    }
    SECTION("Getting Async RGB-Channels") {
        idx.type = RgbChannel::RgbwAsync;
        refColor = &refColor2;
    }
    SECTION("Getting PWM RGB-Strips") {
        idx.type = RgbChannel::RgbwPwm;
        refColor = &refColor3;
    }
    // SECTION("Getting I2C-Expander") {}
    SECTION("Getting Effect channels") {
        idx.type = RgbChannel::EffectProcessor;
        refColor = &refColor4;
    }

    char buff[512];
    dut.ReadValue(idx, (uint8_t *)&msg, sizeof(ColorMsg_t));
    sprintf(buff, "Returned Color: 0x%02x, 0x%02x, 0x%02x, 0x%02x", msg.red, msg.green, msg.blue, msg.white );
    INFO(buff);
    Color_t retC = {.red = msg.red, .green = msg.green, .blue = msg.blue, .white = msg.white};
    CHECK(ColorIsEqual(&retC, refColor));
}