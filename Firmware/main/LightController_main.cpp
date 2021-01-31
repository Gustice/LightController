/**
 * @file LightController_main.cpp
 * @author Gustice
 * @brief Illumination Platform for almost any purpose
 * @version 0.1
 * @date 2020-05-08
 *
 * @copyright Copyright (c) 2020
 *
 * @todo HAL-Port-Classes should countain debug information (some already have)
 *
 */
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Apa102.h"
#include "Color.h"
#include "Encoder.h"
#include "MyWeb.h"
#include "Pca9685.h"
#include "PcbHardware.h"
#include "RgbwStrip.h"
#include "RotatingIndex.h"
#include "SoftAp.h"
#include "Ws2812.h"
#include "ParamReader.h"

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

/// Tag for logging messages:
const char *cModTag = "Main";

typedef enum ColorChannels {
    RGB,
    RGBW,
    Gray,
};

typedef enum DeviceStartMode {
    RunDemo,
    StartDark,
    StartImage,
};

typedef struct stripConfig_def {
    uint8_t LedCount;
    uint16_t Intensity;
    ColorChannels Channels;
} stripConfig_t;

typedef struct syncLedConfig_def {
    stripConfig_t Strip;
    Color_t Color;
} syncLedConfig_t;

typedef struct asyncLedConfig_def {
    stripConfig_t Strip;
    Color_t Color;
} asyncLedConfig_t;

typedef struct rgbwLedConfig_def {
    stripConfig_t Strip;
    Color_t Color;
} rgbwLedConfig_t;

typedef struct i2cExpander_def {
    stripConfig_t Device;
    uint8_t Address;
    uint16_t GrayValues[16];
} i2cExpander_t;

typedef struct deviceConfig_def {
    DeviceStartMode StartUpMode;
    syncLedConfig_t SyncLeds;
    asyncLedConfig_t AsyncLeds;
    rgbwLedConfig_t RgbStrip;
    i2cExpander_t I2cExpander;
} deviceConfig_t;

/// Configuration for

static deviceConfig_t deviceConfig = {
    .StartUpMode = DeviceStartMode::RunDemo,
    .SyncLeds{
        .Strip{
            .LedCount = 6,
            .Intensity = 0x10,
            .Channels = ColorChannels::RGB,
        },
        .Color = {0, 0, 0, 0}, // so black
    },
    .AsyncLeds{
        .Strip{
            .LedCount = 24,
            .Intensity = 0x10,
            .Channels = ColorChannels::RGBW,
        },
        .Color = {0, 0, 0, 0}, // so black
    },
    .RgbStrip{
        .Strip{
            .LedCount = 6,
            .Intensity = 0x1000,
            .Channels = ColorChannels::RGBW,
        },
        .Color{0, 0, 0, 0}, // so black
    },
    .I2cExpander{
        .Device{
            .LedCount = 3,
            .Intensity = 0x1000,
            .Channels = ColorChannels::Gray,
        },
        .Address = 1,
        .GrayValues =
            {
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
            },
    },
};

factoryInfo_t factoryCfg;

/**
 * @brief Configuration for wifi
 * @details Applied in Access-Point-Mode. Device provides an WiFi Access in this case.
 */
static wifiApConfig_t apConfig = {
    "cLight",   // ssid
    "FiatLuxx", // password
    2,          // max_connection
};              // Connect: http://192.168.4.1/Setup

Apa102 *SLedStrip;
Ws2812 *ALedStrip;
Pca9685 *LedDriver;
RgbwStrip *LedStrip;

const int colorsCnt = 7;
const int stripLen = 24;
const int grayCnt = 16;

SemaphoreHandle_t xNewWebCommand = NULL;
QueueHandle_t xColorQueue;
QueueHandle_t xGrayQueue;

typedef struct Indexes_def {
    size_t ImageSize;
    Color_t *Image;
    RotatingIndex Led;
    RotatingIndex Color;
} Indexes_t;

void RunLEDdemo(void) {
    const Color_t *colors[colorsCnt]{
        &color_Red,
        &color_Green,
        &color_Blue,
        &color_Cyan,
        &color_Magenta,
        &color_Yellow,
        &color_White,
    };

    Color_t syncImage[deviceConfig.SyncLeds.Strip.LedCount];
    Indexes_t syncPort{
        .ImageSize = sizeof(Color_t) * deviceConfig.SyncLeds.Strip.LedCount,
        .Image = syncImage,
        .Led = RotatingIndex(deviceConfig.SyncLeds.Strip.LedCount),
        .Color = RotatingIndex(colorsCnt),
    };

    Color_t asyncImage[deviceConfig.AsyncLeds.Strip.LedCount];
    Indexes_t asyncPort{
        .ImageSize = sizeof(Color_t) * deviceConfig.AsyncLeds.Strip.LedCount,
        .Image = asyncImage,
        .Led = RotatingIndex(deviceConfig.AsyncLeds.Strip.LedCount),
        .Color = RotatingIndex(colorsCnt),
    };

    Color_t rgbImage;
    Indexes_t rgbPort{
        .ImageSize = sizeof(Color_t),
        .Image = &rgbImage,
        .Led = RotatingIndex(1),
        .Color = RotatingIndex(colorsCnt),
    };

    uint16_t grayValues[grayCnt];
    memset(grayValues, 0, sizeof(uint16_t) * grayCnt);
    RotatingIndex rgbColorIdx(colorsCnt);

    while (xSemaphoreTake(xNewWebCommand, (TickType_t)10) != pdTRUE) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        memset(syncImage, 0, syncPort.ImageSize);
        memset(asyncImage, 0, asyncPort.ImageSize);

        memcpy(&syncImage[syncPort.Led.GetIndex()], colors[syncPort.Color.GetIndex()],
            sizeof(Color_t));
        if (syncPort.Led.PeekRevolution()) {
            syncPort.Color.GetIndexAndInkrement();
        }
        syncPort.Led.GetIndexAndInkrement();

        memcpy(&asyncImage[asyncPort.Led.GetIndex()], colors[asyncPort.Color.GetIndex()],
            sizeof(Color_t));
        if (asyncPort.Led.PeekRevolution()) {
            asyncPort.Color.GetIndexAndInkrement();
        }
        auto aIdx = asyncPort.Led.GetIndex();
        asyncPort.Led.GetIndexAndInkrement();
        
        asyncImage[aIdx].red = asyncImage[aIdx].red >> 5;
        asyncImage[aIdx].green = asyncImage[aIdx].green >> 5;
        asyncImage[aIdx].blue = asyncImage[aIdx].blue >> 5;
        asyncImage[aIdx].white = asyncImage[aIdx].white >> 5;

        SLedStrip->SendImage(syncImage);
        ALedStrip->SendImage(asyncImage);
        LedStrip->SetImage(colors[rgbPort.Color.GetIndexAndInkrement()]);
        const Color_t * c = colors[rgbColorIdx.GetIndexAndInkrement()];
        grayValues[0] = (uint16_t)c->red << 4;
        grayValues[1] = (uint16_t)c->green << 4;
        grayValues[2] = (uint16_t)c->blue << 4;
        grayValues[3] = (uint16_t)c->white << 4;
        LedDriver->SendImage(grayValues);
    }
    ESP_LOGI(cModTag, "First Message received");
}

/**
 * @brief LED-Task
 * @details Task for managing LED-Output. Receives processed Colors and applies them
 *      to LED ports
 * @param pvParameters Parameter given at task creation
 */
static void vRefreshLed(void *pvParameters) {
    Color_t image[stripLen];
    uint16_t grayValues[grayCnt];

    memset(image, 0, sizeof(image));
    SLedStrip->SetBrightness(0x20);

    if (deviceConfig.StartUpMode == DeviceStartMode::RunDemo)
        RunLEDdemo(); // This can take forever if demo is not aborted

    if (xColorQueue != 0 && xGrayQueue != 0) { // Task should have start without correct pointer
        ESP_LOGI(cModTag, "Starting LED-Refresh-Cycle");

        for (;;) {
            ColorMsg_t xColorMsg;
            GrayValMsg_t xGrayMsg;

            if (xQueueGenericReceive(xColorQueue, &xColorMsg, (TickType_t)1, pdFALSE) == pdTRUE) {

                ESP_LOGI(cModTag, "New Color r%x g%x b%x w%x i%x", xColorMsg.red, xColorMsg.green,
                    xColorMsg.blue, xColorMsg.white, xColorMsg.intensity);

                image[0].red = xColorMsg.red;
                image[0].green = xColorMsg.green;
                image[0].blue = xColorMsg.blue;
                image[0].white = xColorMsg.white;

                switch (xColorMsg.channel) {
                case RgbChannel::RgbiSync:
                    SLedStrip->SendImage(image);
                    break;

                case RgbChannel::RgbwAsync:
                    image[0].red = image[0].red >> 5;
                    image[0].green = image[0].green >> 5;
                    image[0].blue = image[0].blue >> 5;
                    image[0].white = image[0].white >> 5;
                    ALedStrip->SendImage(image);
                    break;

                case RgbChannel::RgbwPwm:
                    LedStrip->SetImage(&image[0]);
                    break;

                default:
                    break;
                }
            }
            if (xQueueGenericReceive(xGrayQueue, &xGrayMsg, (TickType_t)1, pdFALSE) == pdTRUE) {
                ESP_LOGI(cModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d",
                    xGrayMsg.gray[0], xGrayMsg.gray[1], xGrayMsg.gray[2], xGrayMsg.gray[3],
                    xGrayMsg.gray[4], xGrayMsg.gray[5], xGrayMsg.gray[6], xGrayMsg.gray[7]);
                ESP_LOGI(cModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d",
                    xGrayMsg.gray[8], xGrayMsg.gray[9], xGrayMsg.gray[10], xGrayMsg.gray[11],
                    xGrayMsg.gray[12], xGrayMsg.gray[13], xGrayMsg.gray[14], xGrayMsg.gray[15]);

                for (size_t i = 0; i < grayCnt; i++) {
                    grayValues[i] = (uint16_t)xGrayMsg.gray[i] << 4;
                }

                LedDriver->SendImage(grayValues);
            }
        }
    }
}

QuadDecoder *Encoder;
static void IRAM_ATTR gpio_isr_handler(void *arg) { Encoder->EvalStepSync(); }

/**
 * @brief Enter funktion for underlying OS
 *
 */
void app_main(void) {

    ESP_LOGI(cModTag, "Spiffs works");
    Fs_SetupSpiFFs();

    Fs_ReadFactoryConfiguration(&factoryCfg);
    ESP_LOGI(cModTag, "Read Factory Config:\n\tSN: %s\n\tHW: %s\n\tDev: %s", 
    factoryCfg.SerialNumber, factoryCfg.HwVersion, factoryCfg.DeviceType );

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    static InputPort sw1(Sw1Pin);
    static InputPort sw2(Sw2Pin);
    static InputPort sdCard(SdDetect);

    static SpiPort spi(SpiPort::SpiPorts::HSpi, SyncDataOutPin, gpio_num_t::GPIO_NUM_NC,
        SyncClockOutPin, spi_mode_t::mode3);
    static RmtPort rmt(rmt_channel_t::RMT_CHANNEL_0, GPIO_NUM_21, RmtPort::RmtProtocols::RtzWs2812);
    static UartPort uart(UartTxPin, UartRxPin);

    static I2cPort i2c(0, I2cDataPin, I2cClockPin);

    static PwmPort pwmR(ledc_channel_t::LEDC_CHANNEL_0, LedRedPwmPin);
    static PwmPort pwmG(ledc_channel_t::LEDC_CHANNEL_1, LedGreenPwmPin);
    static PwmPort pwmB(ledc_channel_t::LEDC_CHANNEL_2, LedBluePwmPin);
    static PwmPort pwmW(ledc_channel_t::LEDC_CHANNEL_3, LedWhitePwmPin);

    static Apa102 sledStrip(&spi, 6);
    static Ws2812 aLedStrip(&rmt, 24);
    static Pca9685 ledDriver(&i2c, 0x40);
    static RgbwStrip ledStrip(&pwmR, &pwmG, &pwmB, &pwmW);

    SLedStrip = &sledStrip;
    ALedStrip = &aLedStrip;
    LedDriver = &ledDriver;
    LedStrip = &ledStrip;

    SetupSoftAccessPoint(&apConfig);
    if (sw1.ReadPort() == 0) {
        ResetWiFiConfig();
        ESP_LOGD(cModTag, "Reset wifi parameter");
    }

    DacPort dac1(dac_channel_t::DAC_CHANNEL_1);
    DacPort dac2(dac_channel_t::DAC_CHANNEL_2);
    AdcPort adc1(adc1_channel_t::ADC1_CHANNEL_6);
    AdcPort adc2(adc1_channel_t::ADC1_CHANNEL_7);

    static TriggeredInput encInA(EncoderPinAPin);
    static TriggeredInput encInB(EncoderPinBPin);
    encInA.RegisterIsr(gpio_isr_handler);
    encInB.RegisterIsr(gpio_isr_handler);
    static QuadDecoder encoder(encInA, encInB);
    Encoder = &encoder;

    OutputPort RStatLed(RedStatusLedPin, GpioPort::OutputLogic::Inverted);
    OutputPort BStatLed(BlueStatusLedPin, GpioPort::OutputLogic::Inverted);

    xNewWebCommand = xSemaphoreCreateBinary();
    xColorQueue = xQueueCreate(3, sizeof(ColorMsg_t));
    xGrayQueue = xQueueCreate(3, sizeof(GrayValMsg_t));

    if (xNewWebCommand != NULL && xColorQueue != NULL && xGrayQueue != NULL) {
        static uint8_t taskParam; // Can pass arguments to Task here
        SetupMyWeb(xColorQueue, xGrayQueue, xNewWebCommand);
        xTaskCreate(vRefreshLed, "RefreshLed", 4096, (void *)taskParam, tskIDLE_PRIORITY, NULL);
    } else {
        ESP_LOGE(cModTag,
            "Error creating the dynamic objects. LED task relies on it and cannot be created");
        ESP_LOGD(
            cModTag, "Return Pointer: ColorQueue = %p ; GrayQueue = %p ", xColorQueue, xGrayQueue);
    }

    char outString[] = "Test";
    char inString[20];
    memset(inString, 0, sizeof(inString));

    int lastEncVal = Encoder->GetValue();
    for (;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);

        int encVal = Encoder->GetValue();
        if (encVal != lastEncVal)
            ESP_LOGI(cModTag, "Encoder Value = %d (Errors = %d)", Encoder->GetValue(),
                Encoder->GetErrors());
        lastEncVal = encVal;

        RStatLed.WritePort(sw1.ReadPort());
        BStatLed.WritePort(sw2.ReadPort());

        uint16_t vin1 = adc1.ReadPort();
        uint16_t vin2 = adc2.ReadPort();
        // ESP_LOGI(cModTag, "Voltag1 = %d;   Voltage2 = %d", vin1, vin2);

        int len = uart.ReceiveSync((uint8_t *)inString, 20);
        uart.TransmitSync((uint8_t *)outString, 4);

        //     vTaskDelay(1000 / portTICK_PERIOD_MS);

        dac1.WritePort((uint8_t)(vin1 >> 4));
        dac2.WritePort((uint8_t)(vin2 >> 4));
    }
}
