#pragma once

#include <stdint.h>

#include "GpioPort.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"

extern uint64_t virtualGpioPort;
void Mock_setNextConfigReturn(esp_err_t state );
gpio_config_t MockGetPortConfig(void);