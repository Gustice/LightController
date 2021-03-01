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
#include "MyWeb.h"
#include "ParamReader.h"
#include "Pca9685.h"
#include "PcbHardware.h"
#include "RgbwStrip.h"
#include "RotatingIndex.h"
#include "SoftAp.h"
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
QueueHandle_t xColorQueue;
QueueHandle_t xGrayQueue;

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
        Device->SetupDemo();
        while (xSemaphoreTake(xNewWebCommand, (TickType_t)10) != pdTRUE) {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(ModTag, "First Message received");
    }

    if (xColorQueue != 0 && xGrayQueue != 0) { // Task should have start without correct pointer
        ESP_LOGI(ModTag, "Starting LED-Refresh-Cycle");

        for (;;) {
            ColorMsg_t xColorMsg;
            GrayValMsg_t xGrayMsg;

            if (xQueueGenericReceive(xColorQueue, &xColorMsg, (TickType_t)1, pdFALSE) == pdTRUE) {
                ESP_LOGI(ModTag, "New Color r%x g%x b%x w%x i%x to Channel %d", xColorMsg.red,
                    xColorMsg.green, xColorMsg.blue, xColorMsg.white, xColorMsg.intensity,
                    xColorMsg.channel);
                Device->ApplyRgbColorMessage(&xColorMsg);
            }

            if (xQueueGenericReceive(xGrayQueue, &xGrayMsg, (TickType_t)1, pdFALSE) == pdTRUE) {
                ESP_LOGI(ModTag, "Light control: Ch1..8 = %d, %d, %d, %d, %d, %d, %d, %d",
                    xGrayMsg.gray[0], xGrayMsg.gray[1], xGrayMsg.gray[2], xGrayMsg.gray[3],
                    xGrayMsg.gray[4], xGrayMsg.gray[5], xGrayMsg.gray[6], xGrayMsg.gray[7]);
                ESP_LOGI(ModTag, "Light control: Ch9..16 = %d, %d, %d, %d, %d, %d, %d, %d",
                    xGrayMsg.gray[8], xGrayMsg.gray[9], xGrayMsg.gray[10], xGrayMsg.gray[11],
                    xGrayMsg.gray[12], xGrayMsg.gray[13], xGrayMsg.gray[14], xGrayMsg.gray[15]);
                Device->ApplyGrayValueMessage(&xGrayMsg);
            }
        }
    }
}

QuadDecoder *Encoder;
static void IRAM_ATTR gpio_isr_handler(void *arg) { Encoder->EvalStepSync(); }

static esp_err_t GetChannelSettings(ReqColorIdx_t channel, uint8_t *data, size_t length) {
    if (Device == nullptr)
        ESP_LOGE(ModTag, "Device not defined - Cannot read Lights");

    ESP_LOGI(ModTag, "Requesting Channel data from Gate: '%d', Ch=%d / Port=%d", channel.type,
        channel.chIdx, channel.portIdx);

    return Device->ReadValue(channel, data, length);
    ;
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

    // ESP_ERROR_CHECK(nvs_flash_init());
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
        static Apa102 sledStrip(&spi, 32);
        static Ws2812 aLedStrip(&rmt, 32);
        static RgbwStrip ledStrip(&pwmR, &pwmG, &pwmB, &pwmW);
        static Pca9685 ledDriver(&i2c, 0x40);

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

    OutputPort RStatLed(RedStatusLedPin, GpioPort::OutputLogic::Inverted);
    OutputPort BStatLed(BlueStatusLedPin, GpioPort::OutputLogic::Inverted);

    xNewWebCommand = xSemaphoreCreateBinary();
    xColorQueue = xQueueCreate(3, sizeof(ColorMsg_t));
    xGrayQueue = xQueueCreate(3, sizeof(GrayValMsg_t));

    if (xNewWebCommand != NULL && xColorQueue != NULL && xGrayQueue != NULL) {
        static uint8_t taskParam;
        SetupMyWeb(xColorQueue, xGrayQueue, xNewWebCommand, GetChannelSettings);
        xTaskCreate(vRefreshLed, "RefreshLed", 4096, (void *)taskParam, tskIDLE_PRIORITY, NULL);
    } else {
        ESP_LOGE(ModTag,
            "Error creating the dynamic objects. LED task relies on it and cannot be created");
        ESP_LOGD(
            ModTag, "Return Pointer: ColorQueue = %p ; GrayQueue = %p ", xColorQueue, xGrayQueue);
    }

    char outString[] = "Test";
    char inString[20];
    memset(inString, 0, sizeof(inString));

    int lastEncVal = Encoder->GetValue();
    for (;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);

        int encVal = Encoder->GetValue();
        if (encVal != lastEncVal)
            ESP_LOGI(ModTag, "Encoder Value = %d (Errors = %d)", Encoder->GetValue(),
                Encoder->GetErrors());
        lastEncVal = encVal;

        RStatLed.WritePort(sw1.ReadPort());
        BStatLed.WritePort(sw2.ReadPort());

        uint16_t vin1 = adc1.ReadPort();
        uint16_t vin2 = adc2.ReadPort();
        // ESP_LOGI(ModTag, "Voltag1 = %d;   Voltage2 = %d", vin1, vin2);

        int len = uart.ReceiveSync((uint8_t *)inString, 20);
        uart.TransmitSync((uint8_t *)outString, 4);

        //     vTaskDelay(1000 / portTICK_PERIOD_MS);

        dac1.WritePort((uint8_t)(vin1 >> 4));
        dac2.WritePort((uint8_t)(vin2 >> 4));
    }
}
