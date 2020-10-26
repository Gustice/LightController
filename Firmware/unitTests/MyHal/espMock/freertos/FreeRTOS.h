#pragma once

#include <stdint.h>
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

#define configTICK_RATE_HZ 1000
#define pdMS_TO_TICKS( xTimeInMs ) ( ( ( TickType_t ) ( xTimeInMs ) * configTICK_RATE_HZ ) / ( TickType_t ) 1000 )
#define pdTICKS_TO_MS( xTicks )   ( ( uint32_t ) ( xTicks ) * 1000 / configTICK_RATE_HZ )


#define portTICK_RATE_MS 50