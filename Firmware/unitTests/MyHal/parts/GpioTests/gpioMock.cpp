#include "TestUtils.h"
#include "iGpioInclude.h"

esp_err_t nextConfigReturn = ESP_OK;
void Mock_setNextConfigReturn(esp_err_t state) { nextConfigReturn = state; }

gpio_config_t lastConfig;
gpio_config_t MockGetPortConfig(void) { return lastConfig; }

uint64_t virtualGpioPort = 0;

esp_err_t gpio_reset_pin(gpio_num_t gpio_num) {
    UNSCOPED_INFO("Reset Pin: " << gpio_num);
    return (esp_err_t)0u;
}

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig) {
    memcpy(&lastConfig, pGPIOConfig, sizeof(lastConfig));
    UNSCOPED_INFO("Config Pins: " << pGPIOConfig->pin_bit_mask);
    auto retval = nextConfigReturn;
    nextConfigReturn = ESP_OK;
    return retval;
}

int gpio_get_level(gpio_num_t gpio_num) {
    UNSCOPED_INFO("Read Pin: " << gpio_num);
    return ((virtualGpioPort >> gpio_num) & 1);
    return 0;
}

esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level) {
    UNSCOPED_INFO("Set Pin: " << gpio_num << " to " << level);

    virtualGpioPort &= ~(1 << gpio_num);
    virtualGpioPort |= ((level & 1) << gpio_num);
    return (esp_err_t)0u;
}

esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    lastConfig.intr_type = intr_type;
    return (esp_err_t)0u;
}

esp_err_t gpio_install_isr_service(int intr_alloc_flags)
{
    return (esp_err_t)0u;
}

esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
{
    return (esp_err_t)0u;
}


