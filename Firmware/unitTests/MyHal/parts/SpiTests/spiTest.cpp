#include "TestUtils.h"
#include "iSpiInclude.h"

TEST_CASE("Construction of SPI object as precondition", "[spi]") 
{
    SpiPort dut(SpiPort::SpiPort::HSpi, 
                        spi_mode_t::mode0, GPIO_NUM_27);
}

TEST_CASE("Tranceiving on SPI object", "[spi]")
{ 
    SpiPort dut(SpiPort::SpiPort::HSpi, 
                        spi_mode_t::mode0, GPIO_NUM_27);

    uint8_t rx[2];
    uint8_t tx[2] = {1,2};

    std::memset(virtualSpiTxBuffer, 0, 2);
    virtualSpiRxBuffer[0] = 3;
    virtualSpiRxBuffer[1] = 4;

    dut.TransmitSync(tx, rx, 2);

    REQUIRE ( std::memcmp(tx, virtualSpiTxBuffer, 2) == 0 );
    REQUIRE ( std::memcmp(rx, virtualSpiRxBuffer, 2) == 0 );
}
