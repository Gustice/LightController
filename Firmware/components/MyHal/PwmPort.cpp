/**
 * @file PwmPort.cpp
 * @author Gustice
 * @brief PWM-classes Implementation
 * @version 0.1
 * @date 2020-10-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "iPwmInclude.h"

PwmPort::PwmPort(ledc_channel_t channel, gpio_num_t output) {
    _channel = channel;
    _port = output;
        
    ledc_timer_config_t ledc_timer;

    ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num        = LEDC_TIMER_1;
    ledc_timer.duty_resolution  = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz          = 1000;  // set output frequency at 1k Hz
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel    = channel;
    ledc_channel.timer_sel  = LEDC_TIMER_1;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num   = output;
    ledc_channel.duty       = 0; 
    ledc_channel.hpoint     = 0;
    ledc_channel_config(&ledc_channel);
    _initOk = true;
}

void PwmPort::WritePort(uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel);
}