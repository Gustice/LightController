
#pragma once 

typedef enum {
    PERIPH_LEDC_MODULE = 0,
    PERIPH_UART0_MODULE,
    PERIPH_UART1_MODULE,
    PERIPH_UART2_MODULE,
    PERIPH_I2C0_MODULE,
    PERIPH_I2C1_MODULE,
    PERIPH_I2S0_MODULE,
    PERIPH_I2S1_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_PWM0_MODULE,
    PERIPH_PWM1_MODULE,
    PERIPH_PWM2_MODULE,
    PERIPH_PWM3_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_UHCI1_MODULE,
    PERIPH_RMT_MODULE,
    PERIPH_PCNT_MODULE,
    PERIPH_SPI_MODULE,
    PERIPH_HSPI_MODULE,
    PERIPH_VSPI_MODULE,
    PERIPH_SPI_DMA_MODULE,
    PERIPH_SDMMC_MODULE,
    PERIPH_SDIO_SLAVE_MODULE,
    PERIPH_CAN_MODULE,
    PERIPH_EMAC_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
    PERIPH_BT_BASEBAND_MODULE,
    PERIPH_BT_LC_MODULE,
    PERIPH_AES_MODULE,
    PERIPH_SHA_MODULE,
    PERIPH_RSA_MODULE,
} periph_module_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      enable peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Clock for the module will be ungated, and reset de-asserted.
 *
 * @return     NULL
 *
 */
void periph_module_enable(periph_module_t periph);

/**
 * @brief      disable peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Clock for the module will be gated, reset asserted.
 *
 * @return     NULL
 *
 */
void periph_module_disable(periph_module_t periph);

/**
 * @brief      reset peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Reset will asserted then de-assrted for the peripheral.
 *
 * Calling this function does not enable or disable the clock for the module.
 *
 * @return     NULL
 *
 */
void periph_module_reset(periph_module_t periph);


#ifdef __cplusplus
}
#endif

