/**
 * @file iSpiInclude.h
 * @author Gustice
 * @brief Private SPI-Include file
 * @details Untangles user implementation from driver, for test purpose
 * @version 0.1
 * @date 2020-10-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once 

#include <stdio.h>
#include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_system.h"
#include "SpiPort.h"


esp_err_t SPI_TransmitSync(spi_device_handle_t spi, const uint8_t *txData,  uint8_t *rxData, size_t len);
spi_device_handle_t InitSpiPort(spi_host_device_t host, uint8_t mode, gpio_num_t csPin );
spi_device_handle_t InitSpiPortCustom(spi_host_device_t host, gpio_num_t mosi, gpio_num_t miso, gpio_num_t clk, uint8_t mode, gpio_num_t csPin );
