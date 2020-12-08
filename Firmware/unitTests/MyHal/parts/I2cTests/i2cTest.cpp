#include "TestUtils.h"
#include "iI2cInclude.h"

TEST_CASE("Construction of I2C object as precondition", "[i2c]") {
    I2cPort dut(I2cPort::I2cInterfaces::Int1);
    REQUIRE(true);
}

TEST_CASE("Construction of I2C yields successful init", "[i2c]") {
    I2cPort dut(I2cPort::I2cInterfaces::Int1);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Write on virtual I2C port - Writes only defined length", "[i2c]") {
    const int slaveAdd = 1;
    uint8_t txData[6] = {1, 2, 3, 4, 5, 6};
    memset(virtualI2cBuffer, 0, i2cBufSize);

    I2cPort dut(I2cPort::I2cInterfaces::Int1);
    dut.WriteData(slaveAdd, txData, 5);
    REQUIRE(memcmp(txData, virtualI2cBuffer, 5) == 0);
    REQUIRE(virtualI2cBuffer[5] == 0);
}

TEST_CASE("Read on virtual I2C port - Reads only defined length", "[i2c]") {
    const int slaveAdd = 1;
    uint8_t rxData[6] = {11, 12, 13, 14, 15, 16};
    memcpy(virtualI2cBuffer, rxData, sizeof(rxData));
    memset(rxData, 0, sizeof(rxData));

    I2cPort dut(I2cPort::I2cInterfaces::Int1);
    dut.ReadData(slaveAdd, rxData, 5);
    REQUIRE(memcmp(rxData, virtualI2cBuffer, 5) == 0);
    REQUIRE(rxData[5] == 0);
}
