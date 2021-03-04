#pragma once

#include "../esp_err.h"
#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

#define portCHAR		int8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	int

typedef portBASE_TYPE			BaseType_t;

typedef void * QueueHandle_t;
typedef QueueHandle_t SemaphoreHandle_t;
typedef uint32_t TickType_t;

esp_err_t xQueueSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition );
esp_err_t xQueueSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait );
