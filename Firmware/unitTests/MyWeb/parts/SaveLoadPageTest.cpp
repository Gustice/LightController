#include "ProcessHandler.h"
#include "TestUtils.h"

extern char FileBuffer[];
extern const size_t FileBufferSize;
extern char FileName[];
extern size_t FileSize;


static ColorMsg_t NextColorObj;
static GrayValMsg_t NextGrayValObj;
static ReqColorIdx_t LastChTarget;
extern ColorMsg_t LastColorMsg;
extern GrayValMsg_t LastGrayValMsg;


static esp_err_t GetChannelSettings(ReqColorIdx_t channel, uint8_t * data, size_t length)
{
    if (channel.type == RgbChannel::I2cExpanderPwm)
    {
        memcpy(&LastChTarget, &channel, sizeof(ReqColorIdx_t));
        memcpy(data, &NextGrayValObj, length);
        return ESP_OK;
    }
    
    memcpy(&LastChTarget, &channel, sizeof(ReqColorIdx_t));
    memcpy(data, &NextColorObj, length);
    return ESP_OK;
}

static deviceConfig_t global_config {
    .SyncLeds {
        .Strip {
            .Channels = ColorChannels_t::RGBW,
        }
    },
    .AsyncLeds {
        .Strip {
            .Channels = ColorChannels_t::RGBW,
        }
    },
    .RgbStrip {
        .Strip {
            .Channels = ColorChannels_t::RGBW,
        }
    },
    .I2cExpander {
        .Device {
            .LedCount = 16,
        }
    }
};

/** "POST /api/SaveToPage" */
const char * testStream1 = "Ch1: R:0x11 G:0x12 B:0x13 W:0x14 \nCh2: R:0x11 G:0x12 B:0x13 W:0x14 \n";
/** "POST /api/SetPort/RGBWAsync" */
TEST_CASE("Save to Page 1", "[Pages]") {
    memset(FileBuffer, 0, FileBufferSize);
    const char *Payload = "{\"Page\":1}";
    const char *output;

    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, GetChannelSettings, &global_config);

    NextColorObj.red = 0x11;
    NextColorObj.green = 0x12;
    NextColorObj.blue = 0x13;
    NextColorObj.white = 0x14;
    bool result = ProcessSaveToPage(Payload, &output);
    
    printf("FileBuffer: \n%s", FileBuffer);
    printf("testStream: \n%s", testStream1);
    CHECK( (0 == strcmp(FileBuffer, testStream1)) );
}


const char * testStream2 = "Ch1: R:0x31 G:0x32 B:0x33 W:0x34 \nCh2: R:0x41 G:0x42 B:0x43 W:0x44 \n";
/** "POST /api/SetPort/RGBWAsync" */
TEST_CASE("Load from Page 1", "[Pages]") {
    memset(FileBuffer, 0, FileBufferSize);
    strcpy(FileBuffer, testStream2);
    FileSize = strlen(testStream2);
    const char *Payload = "{\"Page\":1}";
    
    const char *output;

    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, GetChannelSettings, &global_config);
    bool result = ProcessLoadPage(Payload, &output);

}

/** "POST /api/ResetSavedPages" */
TEST_CASE("Post ResetPages Handler-Tests", "[ConfigPost]") {
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, (QueueHandle_t)2, nullptr , &global_config);
    bool result = ProcessResetPages(nullptr, &output);
}
