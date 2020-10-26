/**
 * @file SpiPort.cpp
 * @author Gustice
 * @brief I2C-Port classes implementation
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "iSpiInclude.h"

#define SPI_TRANSMIT_RECEIVE (SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA)

esp_err_t SPI_TransmitSync(spi_device_handle_t spi, const uint8_t *txData, uint8_t *rxData,
                           size_t len) {
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0)
        return ESP_OK;

    memset(&t, 0, sizeof(t));
    t.flags = 0;
    t.length = len * 8;
    t.tx_buffer = txData;
    t.rx_buffer = rxData;
    t.user = (void *)1;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);

    return ret;
}

// This function is called (in irq context!) just before a transmission starts. It will
// set the D/C line to the value indicated in the user field.
void spi_pre_transfer_callback(spi_transaction_t *t) {
    // int dc=(int)t->user;
    // Can be used for custom actions
}

#define DMA_CHAN 2

static const spi_bus_config_t interfaces[4] = {
    /* mosi,       miso,              sclk, quadPin1,    quadPin2,    estimatedRate */
    {GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_6, GPIO_NUM_NC, GPIO_NUM_NC, 320 * 2 + 8, 0u, 0},    // SPI
    {GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_NC, GPIO_NUM_NC, 320 * 2 + 8, 0u, 0}, // HSPI
    {GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_NC, GPIO_NUM_NC, 320 * 2 + 8, 0u, 0}, // VSPI
    {GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, 320 * 2 + 8, 0u, 0}, // Custom
};

spi_device_handle_t InitSpiPortCustom(spi_host_device_t host, gpio_num_t mosi, gpio_num_t miso,
                                      gpio_num_t clk, uint8_t mode, gpio_num_t csPin) {
    esp_err_t ret;
    spi_device_handle_t spi;

    spi_bus_config_t buscfg {
        .mosi_io_num = mosi,
        .miso_io_num = miso,
        .sclk_io_num = clk,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = 320 * 2 + 8,
        .flags = 0,
        .intr_flags = 0,
    };
    
    spi_device_interface_config_t devcfg {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 2 * 1000 * 1000, // Clock out at 2 MHz
        .input_delay_ns = 0,
        .spics_io_num = csPin,         // CS pin
        .flags = 0,
        .queue_size = 7, // queue up to 7 transactions at a time
        .pre_cb = spi_pre_transfer_callback, // pre-transfer callback to handle D/C line
        .post_cb = NULL, // pre-transfer callback to handle D/C line
    };
    
    // Initialize the SPI bus
    ret = spi_bus_initialize(host, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(host, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    return spi;
}

spi_device_handle_t InitSpiPort(spi_host_device_t host, uint8_t mode, gpio_num_t csPin) {
    esp_err_t ret;
    int configPage = host - SPI1_HOST;
    spi_device_handle_t spi;

    spi_bus_config_t buscfg;
    memcpy(&buscfg, &interfaces[configPage], sizeof(spi_bus_config_t));

    spi_device_interface_config_t devcfg {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = mode,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 2 * 1000 * 1000, // Clock out at 2 MHz
        .input_delay_ns = 0,
        .spics_io_num = csPin,         // CS pin
        .flags = 0,
        .queue_size = 7, // queue up to 7 transactions at a time
        .pre_cb = spi_pre_transfer_callback, // pre-transfer callback to handle D/C line
        .post_cb = NULL, // pre-transfer callback to handle D/C line
    };

    // Initialize the SPI bus
    ret = spi_bus_initialize(host, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(host, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    return spi;
}

SpiPort::SpiPort(SpiPorts interface, spi_mode_t mode, gpio_num_t csPort) : PortBase("HAL-Spi") {
    _spiH = InitSpiPort((spi_host_device_t)interface, mode, csPort);
    if (_spiH != nullptr) {
        InitOk = true;
    }
}

SpiPort::SpiPort(SpiPorts interface, gpio_num_t mosi, gpio_num_t miso, gpio_num_t clk,
                 spi_mode_t mode, gpio_num_t csPort)
    : PortBase("HAL-Spi") {
    _spiH = InitSpiPortCustom((spi_host_device_t)interface, mosi, miso, clk, mode, csPort);
    if (_spiH != nullptr) {
        InitOk = true;
    }
}

esp_err_t SpiPort::TransmitSync(const uint8_t *txData, uint8_t *rxData, size_t len) {
    return SPI_TransmitSync(_spiH, txData, rxData, len);
}
