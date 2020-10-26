#include "TestUtils.h"
#include "iRmtInclude.h"

TEST_CASE("Construction of RMT object just for fun", "[Rmt]") 
{
    RmtPort dut(rmt_channel_t::RMT_CHANNEL_0, gpio_num_t::GPIO_NUM_0, RmtPort::RmtProtocols::RtzWs2812);
}

TEST_CASE("Check if Protocol is returned correctly", "[Rmt]") 
{
    RmtPort dut(rmt_channel_t::RMT_CHANNEL_0, gpio_num_t::GPIO_NUM_0, RmtPort::RmtProtocols::RtzWs2812);
    REQUIRE( RmtPort::RmtProtocols::RtzWs2812 == dut.GetProtocol());
}

TEST_CASE("Write on virtual RMT port", "[Rmt]") 
{
    RmtPort dut(rmt_channel_t::RMT_CHANNEL_0, gpio_num_t::GPIO_NUM_0, RmtPort::RmtProtocols::RtzWs2812);

    uint8_t buffer[9] = {1,2,3,4,5,6,7,8,9};
    dut.WriteData(buffer, sizeof(buffer));

    REQUIRE( RmtSize == sizeof(buffer));
    REQUIRE( 0 == memcmp(buffer, virtualRmtTxBuffer, sizeof(buffer)));
}

TEST_CASE("Write different sizes and check Timeout", "[Rmt]") 
{
    uint8_t data[8];
    memset(data, 0, sizeof(data));

    RmtPort dut(rmt_channel_t::RMT_CHANNEL_0, gpio_num_t::GPIO_NUM_0, RmtPort::RmtProtocols::RtzWs2812);
    uint8_t buffer[9] = {1,2,3,4,5,6,7,8,9};

    dut.WriteData(buffer, 3);
    REQUIRE( RmtSize == 3 );
    CHECK( RmtTimeout == 33 );

    dut.WriteData(buffer, 6);
    REQUIRE( RmtSize == 6 );
    CHECK( RmtTimeout == 66 );

    dut.WriteData(buffer, 9);
    REQUIRE( RmtSize == 9 );
    CHECK( RmtTimeout == 99 );
}