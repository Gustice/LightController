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


static esp_err_t GetChannelSettings(GetChannelMsg request)
{
    if (request.Target.type == RgbChannel::I2cExpanderPwm)
    {
        memcpy(&LastChTarget, &request.Target, sizeof(ReqColorIdx_t));
        memcpy(request.pStream, &NextGrayValObj, request.PayLoadSize);
        return ESP_OK;
    }
    
    memcpy(&LastChTarget, &request.Target, sizeof(ReqColorIdx_t));
    memcpy(request.pStream, &NextColorObj, request.PayLoadSize);
    return ESP_OK;
}

static deviceConfig_t global_config {
    .SyncLeds {
        .Strip {
            .Channels = ColorChannels::RGBW,
        }
    },
    .AsyncLeds {
        .Strip {
            .Channels = ColorChannels::RGBW,
        }
    },
    .RgbStrip {
        .Strip {
            .Channels = ColorChannels::RGBW,
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

    SetQueueHandlesForPostH((QueueHandle_t)1, GetChannelSettings, &global_config);

    NextColorObj.red = 0x11;
    NextColorObj.green = 0x12;
    NextColorObj.blue = 0x13;
    NextColorObj.white = 0x14;
    bool result = ProcessSaveToPage(Payload, &output);
    
    INFO("FileBuffer: \n"); 
    INFO(FileBuffer)
    INFO("testStream: \n");
    INFO(testStream1);
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
    SetQueueHandlesForPostH((QueueHandle_t)1, GetChannelSettings, &global_config);
    bool result = ProcessLoadPage(Payload, &output);
}

/** "POST /api/ResetSavedPages" */
TEST_CASE("Post ResetPages Handler-Tests", "[ConfigPost]") {    
    const char *output;
    SetQueueHandlesForPostH((QueueHandle_t)1, nullptr , &global_config);
    bool result = ProcessResetPages(nullptr, &output);
}
