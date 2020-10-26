#include "TestUtils.h"
#include "iAdcInclude.h"

uint16_t analogSignal = 0;

TEST_CASE("Construction of ADC object as precondition", "[adc]") {
    AdcPort dut(ADC1_CHANNEL_0);
    REQUIRE(true);
}

TEST_CASE("Construction of ADC yields successful init", "[adc]") {
    AdcPort dut(ADC1_CHANNEL_0);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Read virtual ADC port", "[adc]") {
    analogSignal = 0x0000;
    AdcPort dut(ADC1_CHANNEL_0);
    REQUIRE(dut.ReadPort() == 0x0000);

    analogSignal = 0xFFFF;
    REQUIRE(dut.ReadPort() == 0xFFFF);
}
