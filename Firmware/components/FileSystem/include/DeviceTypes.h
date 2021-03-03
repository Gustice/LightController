#pragma once

typedef struct factoryInfo_def { // @todo the container can be easily overwritten
    char DeviceType[24];
    char SerialNumber[12]; // Something like SN:123ABC
    char HwVersion[16];   // V 1.00.00
    char SwVersion[12];
} factoryInfo_t;

typedef struct wifiConfig_def {
    uint8_t ssid[32];
    /** @note Length must be >= 8 (if not 0) */
    uint8_t password[64];
    uint8_t max_connection;
} WifiConfig_t;

typedef enum ColorChannels {
    NoChannel = 0,
    RGBI,
    RGB,
    RGBW,
    Gray,
} ColorChannels_t;

typedef enum DeviceStartMode {
    StartDark,
    RunDemo,
    StartImage,
} DeviceStartMode_t;

typedef struct stripConfig_def {
    uint16_t LedCount;
    uint16_t Intensity;
    ColorChannels_t Channels;
} stripConfig_t;

typedef struct syncLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
} syncLedConfig_t;

typedef struct asyncLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    Color_t Color;
} aSyncLedConfig_t;

typedef struct rgbwLedConfig_def {
    bool IsActive;
    stripConfig_t Strip;
    uint16_t ChannelCount;
    Color_t Color;
} rgbwLedConfig_t;

typedef struct i2cExpander_def {
    bool IsActive;
    stripConfig_t Device;
    uint16_t Address;
    uint16_t GrayValues[16];
} i2cExpander_t;

typedef struct deviceConfig_def {
    DeviceStartMode_t StartUpMode;
    syncLedConfig_t SyncLeds;
    aSyncLedConfig_t AsyncLeds;
    rgbwLedConfig_t RgbStrip;
    i2cExpander_t I2cExpander;
} deviceConfig_t;