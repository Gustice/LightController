#pragma once

#include <stdint.h>

#include "RmtPort.h"
#include "esp_log.h"

#define IRAM_ATTR 

extern uint8_t virtualRmtTxBuffer[];
extern size_t RmtSize;
extern uint32_t RmtTimeout;

