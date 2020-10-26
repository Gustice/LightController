#include "TestUtils.h"
#include "Pca9685.h"

const size_t LedWordSize = 4;
const uint8_t SlaveAddr = 1;

TEST_CASE("Construction of LED-driver object as precondition", "[pca9685]") {
    I2cPort i2c;
    Pca9685 dut(&i2c, SlaveAddr);
    REQUIRE(true);
}

void PrettyPrintSentData(uint8_t *txData, size_t len);

TEST_CASE("Sending reset causes no visible problems", "[pca9685]") {
    I2cPort i2c;
    Pca9685 dut(&i2c, SlaveAddr);
    dut.Reset();
}

TEST_CASE("Setting frequency causes no visible problems", "[pca9685]") {
    I2cPort i2c;
    Pca9685 dut(&i2c, SlaveAddr);
    dut.SetPwmFreq(200.0f);
}

const size_t LedCount = 16;
const size_t ByteSize = 4;
uint16_t grayValues[LedCount];
TEST_CASE("Sending of one empty Grayvalue-Frame", "[pca9685]") {
    I2cPort i2c;
    Pca9685 dut(&i2c, SlaveAddr);

    dut.SendImage(grayValues);
    size_t payloadSize = LedCount * ByteSize + 1;
    REQUIRE(payloadSize == i2c.LastLen);
    REQUIRE(i2c.SendBuffer[0] == 0x06);

    for (size_t i = 1; i < payloadSize; i++) {
        REQUIRE(i2c.SendBuffer[i] == 0);
    }
}

TEST_CASE("Sending unique Grayvalue-Frame", "[pca9685]") {
    I2cPort i2c;
    Pca9685 dut(&i2c, SlaveAddr);

    for (size_t i = 0; i < LedCount; i++) {
        grayValues[i] = (uint16_t)(0x0100 * (i + 1));
    }

    dut.SendImage(grayValues);
    PrettyPrintSentData(i2c.SendBuffer, i2c.LastLen);

    uint16_t * grayVal = (uint16_t *)(&i2c.SendBuffer[1]);
    for (size_t i = 0; i < LedCount; i++) {
        uint16_t gv = grayVal[i*2+1];
        uint16_t egv = (uint16_t)(grayValues[i] >> 4);
        REQUIRE( gv == egv);
    }
}

typedef struct Apa102Color_def {
    uint8_t Bright;
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
} Apa102Color_t;

void PrettyPrintSentData(uint8_t *txData, size_t len) {
    char buffer[256];

    sprintf(buffer, "    StartAddr 0x%x", txData[0]);
    UNSCOPED_INFO(buffer);
    
    for (int i = 1; i <= len; i += 4) {
        sprintf(buffer, "    LED %4d data: 0x%2x 0x%2x   0x%2x 0x%2x ", i,
                txData[i], txData[i+1], txData[i+2], txData[i+3]);
        UNSCOPED_INFO(buffer);
    }
}