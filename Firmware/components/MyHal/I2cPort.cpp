/**
 * @file I2cPort.cpp
 * @author Gustice
 * @brief I2C-Port classes implementation
 * @version 0.1
 * @date 2020-10-10
 *
 * @copyright Copyright (c) 2020
 */

#include "iI2cInclude.h"

static const i2c_config_t interfaces[2] = {
    {I2C_MODE_MASTER, GPIO_NUM_4, GPIO_NUM_5, false, false, 0},
    {I2C_MODE_MASTER, GPIO_NUM_18, GPIO_NUM_19, false, false, 0},
};

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */

I2cPort::I2cPort(i2c_port_t port, gpio_num_t sda, gpio_num_t scl) : PortBase("HAL-I2c") {
    _port = port;
    int i2c_master_port = port;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = interfaces[port].sda_pullup_en;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = interfaces[port].scl_pullup_en;
    conf.master.clk_speed = 400000;
    i2c_param_config(i2c_master_port, &conf);

    if (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE,
                           I2C_MASTER_TX_BUF_DISABLE, 0) == ESP_OK)
        InitOk = true;
}

I2cPort::I2cPort(i2c_port_t port) : PortBase("HAL-I2c") {
    _port = port;
    int i2c_master_port = port;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = interfaces[port].sda_io_num;
    conf.sda_pullup_en = interfaces[port].sda_pullup_en;
    conf.scl_io_num = interfaces[port].scl_io_num;
    conf.scl_pullup_en = interfaces[port].scl_pullup_en;
    conf.master.clk_speed = 400000;
    i2c_param_config(i2c_master_port, &conf);

    if (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE,
                           I2C_MASTER_TX_BUF_DISABLE, 0) == ESP_OK)
        InitOk = true;
}

esp_err_t I2cPort::WriteData(uint16_t slaveAdd, uint8_t *data, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaveAdd << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, data, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t I2cPort::ReadData(uint16_t slaveAdd, uint8_t *data, size_t size) {
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaveAdd << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data, size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + size - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
