/**
 * @file Pca9685.cpp
 * @author Gustice
 * @brief LED-Driver class implementation
 * @version 0.1
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Pca9685.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for usleepCommand

enum Pca9685Codes {
    PCA9685_MODE1 = 0x00,
    PCA9685_MODE2 = 0x01,
    LED0_ON_L = 0x06,
    LED0_ON_H = 0x07,
    LED0_OFF_L = 0x08,
    LED0_OFF_H = 0x09,
    ALLLED_ON_L = 0xFA,
    ALLLED_ON_H = 0xFB,
    ALLLED_OFF_L = 0xFC,
    ALLLED_OFF_H = 0xFD,
    PCA9685_PRESCALE = 0xFE,
};

enum Pca9685_Mode1Bits {
    Mode1_Sleep = 0x10,
    Mode1_AutoIncrement = 0x20,
    Mode1_Restart = 0x80,
};

enum Pca9685_Mode2Bits {
    Mode2_OutDriver = 0x04,
};


Pca9685::Pca9685(I2cPort *port, uint8_t address) {
    _port = port;
    _address = address & 0x7F;
    Reset();
    SetPwmFreq(200);
}

Pca9685::~Pca9685() {}

void Pca9685::Reset(void) {
    uint8_t mode = Mode1_Restart | Mode1_AutoIncrement;
    writeRegister(PCA9685_MODE1, mode);
    usleep(10000); // 10 ms
}

void Pca9685::SetPwmFreq(float frequency) {
    frequency *= 0.9f; // Correct for overshoot in the frequency setting (see issue #11).
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= frequency;
    prescaleval -= 1;

    uint8_t prescale = (uint8_t)floor(prescaleval + 0.5f);
    uint8_t oldmode;
    readRegister(PCA9685_MODE1, &oldmode);
    uint8_t newmode = (oldmode & 0x7F) | Mode1_Sleep; // sleep + Autoincrement
    writeRegister(PCA9685_MODE1, newmode);            // go to sleep
    writeRegister(PCA9685_PRESCALE, prescale);        // set the prescaler
    writeRegister(PCA9685_MODE1, oldmode);
    writeRegister(PCA9685_MODE2, Mode2_OutDriver);
    usleep(5000); // 5 ms
    //  This sets the MODE1 register to turn on auto increment.
    writeRegister(
        PCA9685_MODE1,
        oldmode | Mode1_Restart |
            Mode1_AutoIncrement); 
}

void Pca9685::SendImage(uint16_t *grayValues) {
    int idx = 0;
    _data[idx++] = LED0_ON_L;
    for (size_t i = 0; i < _count; i++) {
        uint16_t value = grayValues[i] >> 4;
        // Off-Period
        _data[idx++] = 0;
        _data[idx++] = 0;
        // On-Period
        _data[idx++] = (uint8_t)value;
        _data[idx++] = (uint8_t)(value >> 8);
    }
    _port->WriteData(_address, _data, idx);
}

esp_err_t Pca9685::writeRegister(uint8_t reg, uint8_t word) {
    uint8_t data[2];
    data[0] = reg;
    data[1] = word;
    return _port->WriteData(_address, data, 2);
}

esp_err_t Pca9685::readRegister(uint8_t reg, uint8_t *word) {
    esp_err_t err = _port->WriteData(_address, &reg, 1);
    if (err == ESP_OK) {
        err = _port->ReadData(_address, word, 1);
    }
    return err;
}
