/**
 * @file PcbHardware.h
 * @author Gustice
 * @brief Defines used hardware on PCB
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include "MyHal.h"

const gpio_num_t Sw1Pin = gpio_num_t::GPIO_NUM_36;
const gpio_num_t Sw2Pin = gpio_num_t::GPIO_NUM_39;
const gpio_num_t SdDetect = gpio_num_t::GPIO_NUM_2;

const gpio_num_t EncoderPinAPin = gpio_num_t::GPIO_NUM_14;
const gpio_num_t EncoderPinBPin = gpio_num_t::GPIO_NUM_12;
const gpio_num_t EncoderPinSwPin = gpio_num_t::GPIO_NUM_27;

const gpio_num_t RedStatusLedPin = gpio_num_t::GPIO_NUM_33;
const gpio_num_t BlueStatusLedPin = gpio_num_t::GPIO_NUM_32;

const gpio_num_t UartTxPin = gpio_num_t::GPIO_NUM_4;
const gpio_num_t UartRxPin = gpio_num_t::GPIO_NUM_5;

const gpio_num_t SyncDataOutPin = gpio_num_t::GPIO_NUM_22;
const gpio_num_t SyncClockOutPin = gpio_num_t::GPIO_NUM_23;

const gpio_num_t AsyncDataOutPin = gpio_num_t::GPIO_NUM_21;

const gpio_num_t I2cDataPin = gpio_num_t::GPIO_NUM_13;
const gpio_num_t I2cClockPin = gpio_num_t::GPIO_NUM_15;

const gpio_num_t LedRedPwmPin = gpio_num_t::GPIO_NUM_16;
const gpio_num_t LedGreenPwmPin = gpio_num_t::GPIO_NUM_17;
const gpio_num_t LedBluePwmPin = gpio_num_t::GPIO_NUM_18;
const gpio_num_t LedWhitePwmPin = gpio_num_t::GPIO_NUM_19;

// For documentation purpose
// const gpio_num_t AnaIn1Pin = gpio_num_t::GPIO_NUM_34;
// const gpio_num_t AnaIn1Pin = gpio_num_t::GPIO_NUM_35;
// const gpio_num_t AnaOut1Pin = gpio_num_t::GPIO_NUM_25;
// const gpio_num_t AnaOut2Pin = gpio_num_t::GPIO_NUM_26;
