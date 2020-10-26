#include "TestUtils.h"
#include "iDacInclude.h"

uint16_t analogOutSignal;

TEST_CASE("Construction of DAC object just for fun", "[dac]") {
    DacPort dut(DAC_CHANNEL_1);
    REQUIRE(true);
}

TEST_CASE("Construction of DAC yields successful init", "[dac]") {
    DacPort dut(DAC_CHANNEL_1);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Write virtual Dac port", "[adc]") {
    DacPort dut(DAC_CHANNEL_1);
    dut.WritePort(0x00);
    REQUIRE(analogOutSignal == 0x00);

    dut.WritePort(0xFF);
    REQUIRE(analogOutSignal == 0xFF);
}
