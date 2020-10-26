#include "TestUtils.h"
#include "iGpioInclude.h"

/// ###############
/// GPIO-PORT-Tests
TEST_CASE("Successful construction of GPIO object, as precondition", "[gpio]") {
    GpioPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(true);
}

TEST_CASE("Check if initialized port reserves internal pin", "[gpio]") {
    GpioPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.GetReserved() == 0x0001);
}

TEST_CASE("Check if several initialized ports reserve pin", "[gpio]") {
    GpioPort dut1(gpio_num_t::GPIO_NUM_1);
    REQUIRE(dut1.GetReserved() == 0x0002);
    GpioPort dut2(gpio_num_t::GPIO_NUM_2);
    REQUIRE(dut2.GetReserved() == 0x0006);
    GpioPort dut3(gpio_num_t::GPIO_NUM_3);
    REQUIRE(dut3.GetReserved() == 0x000E);

    REQUIRE(dut3.GetReserved() == dut1.GetReserved());
}

/// ################
/// Input-PORT-Tests
TEST_CASE("Construction of GPIO-InputPort as precondition", "[gpio,input]") {
    InputPort dut(gpio_num_t::GPIO_NUM_0, GpioPort::PullResistor::PullDown);
    REQUIRE( true);
}

TEST_CASE("Construction of GPIO-InputPort without Pullup-parameter yields no error",
          "[gpio,input]") {
    InputPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(true);
}

TEST_CASE("Construction of GPIO-Input yields successful init", "[gpio,input]") {
    InputPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Construction of GPIO-Input with invalid parameter yields failed init", "[gpio,input]") {
    Mock_setNextConfigReturn(ESP_FAIL);
    InputPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.GetInitOkState() == false);
}

TEST_CASE("Check if Input port can be read correctly from mocked hardware", "[gpio,input]") {
    InputPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.ReadPort() == 0x0000);
    virtualGpioPort = 1;
    REQUIRE(dut.ReadPort() == 0x0001);
    virtualGpioPort = 0;
}

TEST_CASE("Passed Pullup-parameters get correctly propagated", "[gpio, input]"){
    gpio_config_t cfg;

    InputPort dut1(gpio_num_t::GPIO_NUM_0, GpioPort::PullResistor::floating);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.pull_down_en == GPIO_PULLDOWN_DISABLE);
    REQUIRE(cfg.pull_up_en == GPIO_PULLUP_DISABLE);

    InputPort dut2(gpio_num_t::GPIO_NUM_1, GpioPort::PullResistor::PullUp);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.pull_down_en == GPIO_PULLDOWN_DISABLE);
    REQUIRE(cfg.pull_up_en == GPIO_PULLUP_ENABLE);

    InputPort dut3(gpio_num_t::GPIO_NUM_2, GpioPort::PullResistor::PullDown);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.pull_down_en == GPIO_PULLDOWN_ENABLE);
    REQUIRE(cfg.pull_up_en == GPIO_PULLUP_DISABLE);
}

TEST_CASE("Passed Interupt-parameters get correctly propagated", "[gpio, input]"){
    gpio_config_t cfg;

    InputPort dut1(gpio_num_t::GPIO_NUM_0, GpioPort::PullResistor::floating);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.intr_type == GPIO_INTR_DISABLE);
}

/// #################
/// TriggeredInput-PORT-Tests
TEST_CASE("Construction of GPIO-TriggeredInput as precondition", "[gpio,triggeredInput]") {
    TriggeredInput dut(gpio_num_t::GPIO_NUM_0, GpioPort::PullResistor::PullDown, TriggeredInput::Interrupt::AnyEdge);
    REQUIRE( true);
}

TEST_CASE("Construction of GPIO-TriggeredInput without Pullup-parameter yields no error",
          "[gpio,triggeredInput]") {
    TriggeredInput dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(true);
}

TEST_CASE("Construction of GPIO-TriggeredInput yields successful init", "[gpio,triggeredInput]") {
    TriggeredInput dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Construction of GPIO-TriggeredInput with invalid parameter yields failed init", "[gpio,triggeredInput]") {
    Mock_setNextConfigReturn(ESP_FAIL);
    TriggeredInput dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(dut.GetInitOkState() == false);
}

static void TestIsr (void *) { };
TEST_CASE("Hooking a Interrupt Service to GPIO-TriggeredInput yields no fail", "[gpio,triggeredInput]") {
    TriggeredInput dut(gpio_num_t::GPIO_NUM_0);
    dut.RegisterIsr(TestIsr);
    REQUIRE(dut.GetInitOkState() == true);
}

TEST_CASE("Passed Pullup-parameters on TriggeredInput get correctly propagated", "[gpio,triggeredInput]"){
    gpio_config_t cfg;
    auto pullUpCfg = GpioPort::PullResistor::floating;

    TriggeredInput dut1(gpio_num_t::GPIO_NUM_0, pullUpCfg, TriggeredInput::Interrupt::AnyEdge);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.intr_type == GPIO_INTR_ANYEDGE);

    TriggeredInput dut2(gpio_num_t::GPIO_NUM_1, pullUpCfg, TriggeredInput::Interrupt::NegativeEdge);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.intr_type == GPIO_INTR_NEGEDGE);

    TriggeredInput dut3(gpio_num_t::GPIO_NUM_2, pullUpCfg, TriggeredInput::Interrupt::PositiveEdge);
    cfg = MockGetPortConfig();
    REQUIRE(cfg.intr_type == GPIO_INTR_POSEDGE);
}

/// #################
/// Output-PORT-Tests
TEST_CASE("Construction of GPIO-OutputPort as precondition", "[gpio,output]") {
    OutputPort dut(gpio_num_t::GPIO_NUM_0, GpioPort::OutputLogic::Normal);
    REQUIRE(true);
}

TEST_CASE("Construction of GPIO-OutputPort with no OutputLogic parameter", "[gpio,output]") {
    OutputPort dut(gpio_num_t::GPIO_NUM_0);
    REQUIRE(true);
}

TEST_CASE("Check if Output port can be written correctly on mocked hardware", "[gpio,output]") {
    OutputPort dut(gpio_num_t::GPIO_NUM_0, GpioPort::OutputLogic::Normal);
    REQUIRE(virtualGpioPort == 0);
    dut.WritePort(1);
    REQUIRE(virtualGpioPort == 1);
    dut.WritePort(0);
    REQUIRE(virtualGpioPort == 0);
}
