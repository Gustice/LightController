#include "TestUtils.h"
#include "iPwmInclude.h"

float pulsedOutSignal;

TEST_CASE("Construction of PWM object just for fun", "[pwm]") 
{
    PwmPort dut(ledc_channel_t::LEDC_CHANNEL_0, gpio_num_t::GPIO_NUM_10);
}

TEST_CASE("Write virtual Pwm port", "[adc]") 
{
    PwmPort dut(ledc_channel_t::LEDC_CHANNEL_0, gpio_num_t::GPIO_NUM_16);
    dut.WritePort((uint32_t)0.0f);
    REQUIRE( pulsedOutSignal == 0.0f );
    
    dut.WritePort((uint32_t)50.0f);
    REQUIRE( pulsedOutSignal == 50.0f );
}
