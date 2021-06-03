#include "TestUtils.h"
#include "ParamReader.h"

TEST_CASE("ReadingConfig yields error if module not initializes", "[cJson]")
{
    deviceConfig_t deviceSet;
    factoryInfo_t factorySet;
    REQUIRE ( (Fs_ReadDeviceConfiguration(&deviceSet) == ESP_FAIL) );
    REQUIRE ( (Fs_ReadFactoryConfiguration(&factorySet) == ESP_FAIL) );
}


static bool CheckColor(Color c1, Color c2) {
    if (c1 == c2)
        return true;

    Color_t v1 = c1.GetColor();
    Color_t v2 = c2.GetColor();
    INFO(printf(" rgbw: 0x%02x 0x%02x 0x%02x 0x%02x != 0x%02x 0x%02x 0x%02x 0x%02x\n", v1.red, v1.green, v1.blue,
                v1.white, v2.red, v2.green, v2.blue, v2.white));
    INFO(printf(" rgbw: %d %d %d %d != %d %d %d %d\n", v1.red, v1.green, v1.blue, v1.white, v2.red, v2.green, v2.blue,
                v2.white));
    return false;
}
TEST_CASE("Reading Device configuration", "[cJson]")
{
    Fs_SetupSpiFFs();
    deviceConfig_t deviceSet;
    memset(&deviceSet, 0, sizeof(deviceConfig_t));
    REQUIRE ( Fs_ReadDeviceConfiguration(&deviceSet) == ESP_OK );
    
    CHECK(deviceSet.StartUpMode == DeviceStartMode::RunDemo);

    CHECK(deviceSet.SyncLeds.IsActive == true);
    CHECK(deviceSet.SyncLeds.Strip.LedCount == 6);
    CHECK( (deviceSet.SyncLeds.Strip.Channels == ColorChannels_t::RGBI) );
    CHECK(CheckColor(Color(1, 2, 3, 4), deviceSet.SyncLeds.Color));
    CHECK( 5 == deviceSet.SyncLeds.Delay);

    CHECK(deviceSet.AsyncLeds.IsActive == true);
    CHECK(deviceSet.AsyncLeds.Strip.LedCount == 10);
    CHECK( (deviceSet.AsyncLeds.Strip.Channels == ColorChannels_t::RGBW) );
    CHECK(CheckColor(Color(11, 12, 13, 14), deviceSet.AsyncLeds.Color));
    CHECK( 15 == deviceSet.AsyncLeds.Delay);
    
    CHECK(deviceSet.RgbStrip.IsActive == true);
    CHECK(deviceSet.RgbStrip.Strip.LedCount == 12);
    CHECK( (deviceSet.RgbStrip.Strip.Channels == ColorChannels_t::RGB) );
    CHECK(CheckColor(Color(21, 22, 23, 24), deviceSet.RgbStrip.Color));
    CHECK( 25 == deviceSet.RgbStrip.Delay);
    
    CHECK(deviceSet.I2cExpander.IsActive == true);
    CHECK(deviceSet.I2cExpander.Device.LedCount == 16);
    CHECK( (deviceSet.I2cExpander.Device.Channels == ColorChannels_t::Gray) );


    CHECK(deviceSet.EffectMachines[0].Target  == TargetGate::SyncLed);
    CHECK(deviceSet.EffectMachines[0].ApplyFlags == 0x00000003);
    CHECK(CheckColor(Color(101,102,103,104), deviceSet.EffectMachines[0].Color));
    CHECK( (deviceSet.EffectMachines[0].Delay == 0) );

    CHECK(deviceSet.EffectMachines[1].Target  == TargetGate::AsyncLed);
    CHECK(deviceSet.EffectMachines[1].ApplyFlags == 0x80000000);
    CHECK(CheckColor(Color(111,112,113,114), deviceSet.EffectMachines[1].Color));
    CHECK( (deviceSet.EffectMachines[1].Delay == 32) );

    CHECK(deviceSet.EffectMachines[2].Target  == TargetGate::LedStrip);
    CHECK(deviceSet.EffectMachines[2].ApplyFlags == 0x80018001);
    CHECK(CheckColor(Color(121,122,123,124), deviceSet.EffectMachines[2].Color));
    CHECK( (deviceSet.EffectMachines[2].Delay == 64) );

    CHECK(deviceSet.EffectMachines[3].Target  == TargetGate::I2cExpander);
    CHECK(deviceSet.EffectMachines[3].ApplyFlags == 0x80000001);
    CHECK(CheckColor(Color(131,132,133,134), deviceSet.EffectMachines[3].Color));
    CHECK( (deviceSet.EffectMachines[3].Delay == 128) );
}

static bool stringIsEqual(const char * expected, const char * actual);

TEST_CASE("Reading Wifi and station configuration", "[cJson]")
{
    Fs_SetupSpiFFs();
    factoryInfo_t factoryImage;
    memset(&factoryImage, 0, sizeof(deviceConfig_t));
    REQUIRE ( Fs_ReadFactoryConfiguration(&factoryImage) == ESP_OK );

    CHECK( stringIsEqual("DemoDev-Mk1", factoryImage.DeviceType ) );
    CHECK( stringIsEqual("00000001", factoryImage.SerialNumber ) );
    CHECK( stringIsEqual("V 0.1.0", factoryImage.HwVersion ) );
}

void appendExtension( char * filepath)
{
    if (strchr(filepath, '.') == NULL)
        strcpy(&filepath[strlen(filepath)], ".html");
}

TEST_CASE("Check if missing line endign is correctly recognized", "[strings]")
{
    char buffer[32];

    SECTION( "Without file extension" ) {
        strcpy(buffer, "SomeRoute");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "SomeRoute.html"));
    }
    SECTION( "File extension" ) {
        strcpy(buffer, "SomeFile.txt");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "SomeFile.txt"));
    }
    SECTION( "FilePath in folder" ) {
        strcpy(buffer, "/server/SomeFile.txt");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "/server/SomeFile.txt"));
    }
}

TEST_CASE("Simple cJSON test in Module", "[cJson]") 
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", 42);
    const char *sys_info = cJSON_PrintUnformatted(root);
    REQUIRE( stringIsEqual("{\"raw\":42}", sys_info) );
    free((void *)sys_info);
    cJSON_Delete(root);
}


static bool stringIsEqual(const char * expected, const char * actual)
{
    if(strcmp(expected, actual) == 0)
        return true;

    UNSCOPED_INFO("Expected " << expected);
    UNSCOPED_INFO("Actual " << actual);
    return false;
}