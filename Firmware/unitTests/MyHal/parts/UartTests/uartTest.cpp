#include "TestUtils.h"
#include "iUartInclude.h"

TEST_CASE("Construction of Uart object as precondition", "[uart]") {
    UartPort dut(gpio_num_t::GPIO_NUM_0, gpio_num_t::GPIO_NUM_1);
    REQUIRE(true);
}

TEST_CASE("Write on virtual Uart port - Writes only defined length", "[uart]") {
    const int slaveAdd = 1;
    uint8_t txData[6] = {1, 2, 3, 4, 5, 6};
    memset(virtualUartBuffer, 0, uartBufLen);

    UartPort dut(gpio_num_t::GPIO_NUM_0, gpio_num_t::GPIO_NUM_1);
    dut.TransmitSync(txData, 5);
    REQUIRE(memcmp(txData, virtualUartBuffer, 5) == 0);
    REQUIRE(virtualUartBuffer[5] == 0);
}

TEST_CASE("Read on virtual Uart port - Reads only defined length", "[uart]") {
    const int slaveAdd = 1;
    uint8_t rxData[6] = {11, 12, 13, 14, 15, 16};
    memcpy(virtualUartBuffer, rxData, sizeof(rxData));
    memset(rxData, 0, sizeof(rxData));

    UartPort dut(gpio_num_t::GPIO_NUM_0, gpio_num_t::GPIO_NUM_1);
    dut.ReceiveSync(rxData, 5);
    REQUIRE(memcmp(rxData, virtualUartBuffer, 5) == 0);
    REQUIRE(rxData[5] == 0);
}
