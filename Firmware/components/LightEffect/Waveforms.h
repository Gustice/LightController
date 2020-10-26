#pragma once
#include <stdint.h>

extern const uint16_t cu16_TemplateLength;  ///< Lenght of all subsequent Template definitions
extern const uint8_t gu8_idleIntensity;     ///< Idle intensity for Slopes and Pulses
extern const uint8_t gu8_fullIntensity;     ///< Idle intensity for Slopes and Pulses

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
