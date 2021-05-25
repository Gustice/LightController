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
 * @todo Log-data should be also provided via web-Interface or to an IP-Socket
 * @todo Refactor all lighting-port names. The wording is incoherent.
 *
 */
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "esp_vfs_semihost.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdmmc_cmd.h"
#include <string.h>

#include "lwip/apps/netbiosns.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mdns.h"

#include "Apa102.h"
#include "Color.h"
#include "Encoder.h"
#include "Macro.h"
#include "MyWeb.h"
#include "ParamReader.h"
#include "Pca9685.h"
#include "PcbHardware.h"
#include "Processor.h"
#include "RgbwStrip.h"
#include "RotatingIndex.h"
#include "SoftAp.h"
#include "Waveforms.h"
#include "Ws2812.h"

#include "DeviceDriver.h"
#include "Utils.h"


extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

/// Tag for logging messages:
const char *ModTag = "Main";

/// Configuration for

static deviceConfig_t deviceConfig;

factoryInfo_t factoryCfg;

/**
 * @brief Configuration for wifi
 * @details Applied in Access-Point-Mode. Device provides an WiFi Access in this case.
 */
static wifiConfig_def apConfig = {
    "cLight",   // ssid
    "FiatLux!", // password
    4,          // max_connection
};              // Connect: http://192.168.4.1/Setup


DeviceDriver *Device;

SemaphoreHandle_t xNewWebCommand = NULL;
QueueHandle_t xSetQueue;

// esp_err_t start_rest_server(const char *base_path);

// static void initialise_mdns(void)
// {
//     mdns_init();
//     mdns_hostname_set(CONFIG_EXAMPLE_MDNS_HOST_NAME);
//     mdns_instance_name_set(MDNS_INSTANCE);

//     mdns_txt_item_t serviceTxtData[] = {
//         {"board", "esp32"},
//         {"path", "/"}
//     };

//     ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
//                                      sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
// }

Effect::EffectProcessor *SyncPcs;
Effect::EffectProcessor *AsyncPcs;
Effect::EffectProcessor *RgbPcs;

/**
 * @brief LED-Task
 * @details Task for managing LED-Output. Receives processed Colors and applies them
 *      to LED ports
 * @param pvParameters Parameter given at task creation
 */
static void vRefreshLed(void *pvParameters) {
    if (Device == nullptr)
        ESP_LOGE(ModTag, "Device not defined - Cannot drive Lights");

    if (deviceConfig.StartUpMode == DeviceStartMode::RunDemo) {
        ESP_LOGI(ModTag, " ## Running Demo");
        while (xSemaphoreTake(xNewWebCommand, (TickType_t)10) != pdTRUE) {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            Device->DemoTick();
        }
        ESP_LOGI(ModTag, "First Message received");
    } else if (deviceConfig.StartUpMode == DeviceStartMode::StartImage) {
        ESP_LOGI(ModTag, " ## Starting Default Image");
        while (xSemaphoreTake(xNewWebCommand, (TickType_t)0) != pdTRUE) {
            vTaskDelay(40 / portTICK_PERIOD_MS);
            Device->StartUpTick();
        }
        ESP_LOGI(ModTag, "First Message received");
    }

    if (xSetQueue != 0) { // Task shouldn't have start without correct pointer anyway
        ESP_LOGI(ModTag, "Starting LED-Refresh-Cycle");

        for (;;) {
            SetChannelDto_t xSetMsg;

            if (xQueueGenericReceive(xSetQueue, &xSetMsg, (TickType_t)1, pdFALSE) == pdTRUE) {
                ESP_LOGI(ModTag, "Requesting Channel data from Gate: '%d', Ch=%d / Port=%d",
                    xSetMsg.Target.type, xSetMsg.Target.chIdx, xSetMsg.Target.portIdx);

                Device->SetValue(xSetMsg.Target, xSetMsg.pStream, xSetMsg.PayLoadSize, &xSetMsg.Apply);
            }
        }
    }
}

QuadDecoder *Encoder;
static void IRAM_ATTR gpio_isr_handler(void *arg) { Encoder->EvalStepSync(); }

static esp_err_t GetChannelSettings(GetChannelMsg request) {
    if (Device == nullptr)
        ESP_LOGE(ModTag, "Device not defined - Cannot read Lights");

    ESP_LOGI(ModTag, "Requesting Channel data from Gate: '%d', Ch=%d / Port=%d",
        request.Target.type, request.Target.chIdx, request.Target.portIdx);

    return Device->ReadValue(request.Target, request.pStream, request.GetSize());
}

enum LedMode {
    Off = 0,
    SteadyOn,
    Blinking,
    Flashing,
    DoubleFlash,
    Burst,
    Flicker,
};

struct StatusLed_t {
    OutputPort *port;
    LedMode mode;
};

StatusLed_t WlanLed;
StatusLed_t ErrLed;

static void vManageStatusLeds(void *pvParameters) {
    const uint16_t cStates[]{
        0x0000, // Off
        0xFFFF, // SteadyOn
        0xFF00, // Blinking
        0xF000, // Flashing
        0x8800, // DoubleFlash
        0xAA00, // Burst
        0xAAAA, // Flicker
    };

    if (WlanLed.port == nullptr || ErrLed.port == nullptr) {
        ESP_LOGE(ModTag, "Ports for Status LEDs not initialized");
        return;
    }

    RotatingIndex rI(16);
    while (true) {
        uint16_t s = rI.GetIndexAndInkrement();
        WlanLed.port->WritePort(cStates[(int)WlanLed.mode] >> s & 1);
        ErrLed.port->WritePort(cStates[(int)ErrLed.mode] >> s & 1);
        vTaskDelay(80 / portTICK_PERIOD_MS);
    }
}


/**
 * @brief Enter function for underlying OS
 *
 */
void app_main(void) {

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // initialise_mdns();
    // netbiosns_init();
    // netbiosns_set_name(CONFIG_EXAMPLE_MDNS_HOST_NAME);

    // ESP_ERROR_CHECK(example_connect());
    // ESP_ERROR_CHECK(init_fs());
    // ESP_ERROR_CHECK(start_rest_server(CONFIG_EXAMPLE_WEB_MOUNT_POINT));

    Fs_SetupSpiFFs();
    Fs_ReadFactoryConfiguration(&factoryCfg);
    ESP_LOGI(ModTag, "Read Factory Config:\n\tSN: %s\n\tHW: %s\n\tDev: %s", factoryCfg.SerialNumber,
        factoryCfg.HwVersion, factoryCfg.DeviceType);

    Init_WebFs();

    static InputPort sw1(Sw1Pin);
    static InputPort sw2(Sw2Pin);

    static SpiPort spi(SpiPort::SpiPorts::HSpi, SyncDataOutPin, gpio_num_t::GPIO_NUM_NC,
        SyncClockOutPin, spi_mode_t::mode3);
    static RmtPort rmt(rmt_channel_t::RMT_CHANNEL_0, GPIO_NUM_21, RmtPort::RmtProtocols::RtzWs2812);
    static UartPort uart(UartTxPin, UartRxPin);

    static I2cPort i2c(0, I2cDataPin, I2cClockPin);

    static PwmPort pwmR(ledc_channel_t::LEDC_CHANNEL_0, LedRedPwmPin);
    static PwmPort pwmG(ledc_channel_t::LEDC_CHANNEL_1, LedGreenPwmPin);
    static PwmPort pwmB(ledc_channel_t::LEDC_CHANNEL_2, LedBluePwmPin);
    static PwmPort pwmW(ledc_channel_t::LEDC_CHANNEL_3, LedWhitePwmPin);

    if (Fs_ReadDeviceConfiguration(&deviceConfig) == ESP_OK) {
        ESP_LOGI(ModTag, " ## Found Configuration: ");
        ESP_LOGI(ModTag, "    Sync LEDs En=%d Cnt=:%d", (int)deviceConfig.SyncLeds.IsActive,
            deviceConfig.SyncLeds.Strip.LedCount);
        ESP_LOGI(ModTag, "    Async LEDs En=%d Cnt=:%d", (int)deviceConfig.AsyncLeds.IsActive,
            deviceConfig.AsyncLeds.Strip.LedCount);
        ESP_LOGI(ModTag, "    RgbStrip LEDs En=%d Chn=%d Cnt=:%d",
            (int)deviceConfig.RgbStrip.IsActive, deviceConfig.RgbStrip.ChannelCount,
            deviceConfig.RgbStrip.Strip.LedCount);
        ESP_LOGI(ModTag, "    Expander LEDs En=%d Cnt=:%d", (int)deviceConfig.I2cExpander.IsActive,
            deviceConfig.I2cExpander.Device.LedCount);
        static Apa102 sledStrip(&spi, deviceConfig.SyncLeds.Strip.LedCount);
        static Ws2812 aLedStrip(&rmt, deviceConfig.AsyncLeds.Strip.LedCount);
        static RgbwStrip ledStrip(&pwmR, &pwmG, &pwmB, &pwmW);
        static Pca9685 ledDriver(&i2c, 0x40);

        static Effect::EffectProcessor syncPcs(Effect::cu16_TemplateLength, 8);
        static Effect::EffectProcessor asyncPcs(Effect::cu16_TemplateLength, 8);
        static Effect::EffectProcessor rgbPcs(Effect::cu16_TemplateLength, 8);

        syncPcs.SetEffect(
            Effect::macStartFull, &deviceConfig.SyncLeds.Color, Effect::gu8_fullIntensity);
        ;
        asyncPcs.SetEffect(
            Effect::macStartFull, &deviceConfig.AsyncLeds.Color, Effect::gu8_fullIntensity);
        ;
        rgbPcs.SetEffect(
            Effect::macStartFull, &deviceConfig.RgbStrip.Color, Effect::gu8_fullIntensity);
        ;

        SyncPcs = &syncPcs;
        AsyncPcs = &asyncPcs;
        RgbPcs = &rgbPcs;

        static DeviceDriver device(&sledStrip, &aLedStrip, &ledStrip, &ledDriver, &deviceConfig);
        Device = &device;
    } else {
        Device = nullptr;
        ESP_LOGE(ModTag, "Could not read Device configuration. No Demo or operation possible");
    }

    if (sw1.ReadPort() == 0) {
        ESP_LOGI(ModTag, "Reset wifi parameter");
        ResetWiFiConfig();
    }
    SetupSoftAccessPoint(&apConfig);

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

    static OutputPort RStatLed(RedStatusLedPin, GpioPort::OutputLogic::Inverted);
    static OutputPort BStatLed(BlueStatusLedPin, GpioPort::OutputLogic::Inverted);

    WlanLed.mode = LedMode::Blinking;
    WlanLed.port = &BStatLed;
    ErrLed.mode = LedMode::DoubleFlash;
    ErrLed.port = &RStatLed;
    xTaskCreate(vManageStatusLeds, "StatusLED", 1024, nullptr, tskIDLE_PRIORITY, NULL);


    xNewWebCommand = xSemaphoreCreateBinary();
    xSetQueue = xQueueCreate(3, sizeof(SetChannelDto_t));

    if (xNewWebCommand != NULL && xSetQueue != NULL) {
        static uint8_t taskParam;
        SetupMyWeb(xSetQueue, xNewWebCommand, GetChannelSettings, &deviceConfig);
        xTaskCreate(vRefreshLed, "RefreshLed", 4096, (void *)taskParam, tskIDLE_PRIORITY, NULL);
    } else {
        ESP_LOGE(ModTag,
            "Error creating the dynamic objects. LED task relies on it and cannot be created");
    }

    char outString[] = "Test";
    char inString[20];
    memset(inString, 0, sizeof(inString));

    int lastEncVal = Encoder->GetValue();
    for (;;) {
        vTaskDelay(500 / portTICK_PERIOD_MS);

        int encVal = Encoder->GetValue();
        if (encVal != lastEncVal)
            ESP_LOGI(ModTag, "Encoder Value = %d (Errors = %d)", Encoder->GetValue(),
                Encoder->GetErrors());
        lastEncVal = encVal;

        // RStatLed.WritePort(sw1.ReadPort());
        // BStatLed.WritePort(sw2.ReadPort());

        uint16_t vin1 = adc1.ReadPort();
        uint16_t vin2 = adc2.ReadPort();
        // ESP_LOGI(ModTag, "Voltag1 = %d;   Voltage2 = %d", vin1, vin2);

        int len = uart.ReceiveSync((uint8_t *)inString, 20);
        uart.TransmitSync((uint8_t *)outString, 4);

        if (len != 0)
            ESP_LOGW(ModTag, "Message received: %s", inString);

        dac1.WritePort((uint8_t)(vin1 >> 4));
        dac2.WritePort((uint8_t)(vin2 >> 4));
    }
}
