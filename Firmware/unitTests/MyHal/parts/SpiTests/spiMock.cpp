#include "TestUtils.h"
#include "iSpiInclude.h"

esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config,
                             int dma_chan) {
    UNSCOPED_INFO("SPI-interface is initialized");
    return ESP_OK;
}

esp_err_t spi_bus_add_device(spi_host_device_t host_id,
                             const spi_device_interface_config_t *dev_config,
                             spi_device_handle_t *handle) {
    UNSCOPED_INFO("SPI-interface is configured");
    return ESP_OK;
}

const size_t spiBufLen = 8u;
uint8_t virtualSpiRxBuffer[spiBufLen];
uint8_t virtualSpiTxBuffer[spiBufLen];

esp_err_t spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc) {
    size_t bLen = trans_desc->length / 8;
    if (bLen > spiBufLen)
        return ESP_FAIL;

    memcpy(trans_desc->rx_buffer, virtualSpiRxBuffer, bLen);
    memcpy(virtualSpiTxBuffer, trans_desc->tx_buffer, bLen);

    return ESP_OK;
}
