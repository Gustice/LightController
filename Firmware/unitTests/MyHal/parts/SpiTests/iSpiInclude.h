#pragma once

#include <stdint.h>

#include "SpiPort.h"
#include "esp_log.h"

extern uint8_t virtualSpiRxBuffer[];
extern uint8_t virtualSpiTxBuffer[];
extern const size_t spiBufLen;
