/**
 * @file EffectWaveforms.h
 * @author Gustice
 * @brief Predefined Waveforms that can be put together to a complex Waveform
 * @version 0.6
 * @date 2019-10-01
 *
 * @copyright Copyright (c) 2019
 *
 */
#pragma once
#include <stdint.h>

namespace Effect {

/// Length of all subsequent Template definitions
extern const uint16_t cu16_TemplateLength;
/// Idle intensity for Slopes and Pulses
extern const uint8_t  gu8_idleIntensity;
/// Idle intensity for Slopes and Pulses
extern const uint8_t  gu8_fullIntensity;
/// Headrom for Effects on top of idle brightness
extern const uint8_t  gu8_dynamicRange;
/// Standard cross fade steps
extern const uint8_t  gu8_fadeSteps;

/// Starts from idle \ref gu8_idleIntensity and represents a gaussian impulse  \ref gu8_fullIntensity
extern const uint8_t gau8_offsetPulse[];
/// Starts at zero and rises to idle intensity \ref gu8_idleIntensity
extern const uint8_t gau8_initSlope[];
/// Starts from idle \ref gu8_idleIntensity and rises up to full intensity \ref gu8_fullIntensity
extern const uint8_t gau8_offsetSlope[];
/// Starts at zero and rises to full intensity \ref gu8_fullIntensity
extern const uint8_t gau8_fullSlope[];
/// Starts at zero and represents a gaussian impulse to full intensity \ref gu8_fullIntensity
extern const uint8_t gau8_fullPulse[];

} // namespace Effect