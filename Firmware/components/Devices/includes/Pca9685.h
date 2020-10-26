/**
 * @file Pca9685.h
 * @author Gustice
 * @brief 
 * @version 0.1
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Color.h"
#include "I2cPort.h"

/**
 * @brief I2C-Connected LED-Driver with 16 ports
 * @note This module can also be used to genarate Servo-Motor signals
 */
class Pca9685 {
  private:
    I2cPort *_port;
    uint8_t _address;
	const size_t _count = 16;
  // 16 LED-Channels with 4 bytes each + 4 byte overhead
	uint8_t _data[16*2*2+4]; 

	esp_err_t writeRegister(uint8_t reg, uint8_t data);
	esp_err_t readRegister(uint8_t reg, uint8_t * data);

  public:
    Pca9685(I2cPort * port, uint8_t address);
    ~Pca9685();

    void SendImage(uint16_t * grayValues);
    void Reset();
    void SetPwmFreq(float frequency);
};
