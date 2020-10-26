
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_RMT_CHANNEL_MEM_WORDS (64) /*!< Each channel owns 64 words memory */
#define SOC_RMT_CHANNELS_NUM (8)       /*!< Total 8 channels */

/**
* @brief RMT channel ID
*
*/
typedef enum {
    RMT_CHANNEL_0,  /*!< RMT channel number 0 */
    RMT_CHANNEL_1,  /*!< RMT channel number 1 */
    RMT_CHANNEL_2,  /*!< RMT channel number 2 */
    RMT_CHANNEL_3,  /*!< RMT channel number 3 */
#if SOC_RMT_CHANNELS_NUM > 4
    RMT_CHANNEL_4,  /*!< RMT channel number 4 */
    RMT_CHANNEL_5,  /*!< RMT channel number 5 */
    RMT_CHANNEL_6,  /*!< RMT channel number 6 */
    RMT_CHANNEL_7,  /*!< RMT channel number 7 */
#endif
    RMT_CHANNEL_MAX /*!< Number of RMT channels */
} rmt_channel_t;

/**
 * @brief RMT Internal Memory Owner
 *
 */
typedef enum {
    RMT_MEM_OWNER_TX, /*!< RMT RX mode, RMT transmitter owns the memory block*/
    RMT_MEM_OWNER_RX, /*!< RMT RX mode, RMT receiver owns the memory block*/
    RMT_MEM_OWNER_MAX,
} rmt_mem_owner_t;

/**
 * @brief Clock Source of RMT Channel
 *
 */
typedef enum {
    RMT_BASECLK_REF, /*!< RMT source clock is REF_TICK, 1MHz by default */
    RMT_BASECLK_APB, /*!< RMT source clock is APB CLK, 80Mhz by default */
    RMT_BASECLK_MAX,
} rmt_source_clk_t;

/**
 * @brief RMT Data Mode
 *
 * @note We highly recommended to use MEM mode not FIFO mode since there will be some gotcha in FIFO mode.
 *
 */
typedef enum {
    RMT_DATA_MODE_FIFO, /*<! RMT memory access in FIFO mode */
    RMT_DATA_MODE_MEM,  /*<! RMT memory access in memory mode */
    RMT_DATA_MODE_MAX,
} rmt_data_mode_t;

/**
 * @brief RMT Channel Working Mode (TX or RX)
 *
 */
typedef enum {
    RMT_MODE_TX, /*!< RMT TX mode */
    RMT_MODE_RX, /*!< RMT RX mode */
    RMT_MODE_MAX
} rmt_mode_t;

/**
 * @brief RMT Idle Level
 *
 */
typedef enum {
    RMT_IDLE_LEVEL_LOW,  /*!< RMT TX idle level: low Level */
    RMT_IDLE_LEVEL_HIGH, /*!< RMT TX idle level: high Level */
    RMT_IDLE_LEVEL_MAX,
} rmt_idle_level_t;

/**
 * @brief RMT Carrier Level
 *
 */
typedef enum {
    RMT_CARRIER_LEVEL_LOW,  /*!< RMT carrier wave is modulated for low Level output */
    RMT_CARRIER_LEVEL_HIGH, /*!< RMT carrier wave is modulated for high Level output */
    RMT_CARRIER_LEVEL_MAX
} rmt_carrier_level_t;

/**
 * @brief RMT Channel Status
 *
 */
typedef enum {
    RMT_CHANNEL_UNINIT, /*!< RMT channel uninitialized */
    RMT_CHANNEL_IDLE,   /*!< RMT channel status idle */
    RMT_CHANNEL_BUSY,   /*!< RMT channel status busy */
} rmt_channel_status_t;

/**
* @brief Data struct of RMT channel status
*/
typedef struct {
    rmt_channel_status_t status[RMT_CHANNEL_MAX]; /*!< Store the current status of each channel */
} rmt_channel_status_result_t;

#ifdef __cplusplus
}
#endif
