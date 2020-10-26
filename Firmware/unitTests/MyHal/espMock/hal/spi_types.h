#pragma once

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
} spi_host_device_t;

//alias for different chips
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_HOST    SPI1_HOST
#define HSPI_HOST   SPI2_HOST
#define VSPI_HOST   SPI3_HOST
#elif CONFIG_IDF_TARGET_ESP32S2
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2
#define SPI_HOST    SPI1_HOST
#define FSPI_HOST   SPI2_HOST
#define HSPI_HOST   SPI3_HOST
#endif
