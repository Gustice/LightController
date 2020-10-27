#include "Waveforms.h"

const uint16_t cu16_TemplateLength = 128;
const uint8_t  gu8_idleIntensity   = 0x55;
const uint8_t  gu8_fullIntensity   = 0xFF;


/// Offset Pulse: Gaussian pulse from background illumination with full scale intensity
const uint8_t gau8_offsetPulse[cu16_TemplateLength * 2] = {
    0x55, 0x55, 0x55, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58, 0x58, 0x59, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x61, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C,
    0x6F, 0x71, 0x74, 0x77, 0x7B, 0x7E, 0x81, 0x85, 0x89, 0x8E, 0x92, 0x97, 0x9B, 0xA0, 0xA5, 0xAA, 0xAF, 0xB5, 0xBA, 0xBF, 0xC4, 0xC9, 0xCF, 0xD3, 0xD8, 0xDD,
    0xE1, 0xE6, 0xEA, 0xED, 0xF1, 0xF4, 0xF7, 0xF9, 0xFB, 0xFD, 0xFD, 0xFE, 0xFF, 0xFE, 0xFD, 0xFD, 0xFB, 0xF9, 0xF7, 0xF4, 0xF1, 0xED, 0xEA, 0xE6, 0xE1, 0xDD,
    0xD8, 0xD3, 0xCF, 0xC9, 0xC4, 0xBF, 0xBA, 0xB5, 0xAF, 0xAA, 0xA5, 0xA0, 0x9B, 0x97, 0x92, 0x8E, 0x89, 0x85, 0x81, 0x7E, 0x7B, 0x77, 0x74, 0x71, 0x6F, 0x6C,
    0x6A, 0x68, 0x66, 0x64, 0x62, 0x61, 0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x59, 0x58, 0x58, 0x57, 0x57, 0x57, 0x56, 0x56, 0x56, 0x55, 0x55,

    0x55, 0x55, 0x55, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58, 0x58, 0x59, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x61, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C,
    0x6F, 0x71, 0x74, 0x77, 0x7B, 0x7E, 0x81, 0x85, 0x89, 0x8E, 0x92, 0x97, 0x9B, 0xA0, 0xA5, 0xAA, 0xAF, 0xB5, 0xBA, 0xBF, 0xC4, 0xC9, 0xCF, 0xD3, 0xD8, 0xDD,
    0xE1, 0xE6, 0xEA, 0xED, 0xF1, 0xF4, 0xF7, 0xF9, 0xFB, 0xFD, 0xFD, 0xFE, 0xFF, 0xFE, 0xFD, 0xFD, 0xFB, 0xF9, 0xF7, 0xF4, 0xF1, 0xED, 0xEA, 0xE6, 0xE1, 0xDD,
    0xD8, 0xD3, 0xCF, 0xC9, 0xC4, 0xBF, 0xBA, 0xB5, 0xAF, 0xAA, 0xA5, 0xA0, 0x9B, 0x97, 0x92, 0x8E, 0x89, 0x85, 0x81, 0x7E, 0x7B, 0x77, 0x74, 0x71, 0x6F, 0x6C,
    0x6A, 0x68, 0x66, 0x64, 0x62, 0x61, 0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x59, 0x58, 0x58, 0x57, 0x57, 0x57, 0x56, 0x56, 0x56, 0x55, 0x55,
};

/// Full Pulse: Gaussian pulse from dark to full scale intensity
const uint8_t gau8_fullPulse[cu16_TemplateLength * 2] = {
    0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0D, 0x0E, 0x10, 0x12, 0x14, 0x17, 0x1A, 0x1D, 0x20, 0x23,
    0x27, 0x2B, 0x2F, 0x34, 0x39, 0x3E, 0x43, 0x49, 0x4F, 0x56, 0x5C, 0x63, 0x6A, 0x71, 0x79, 0x80, 0x88, 0x90, 0x98, 0x9F, 0xA7, 0xAF, 0xB7, 0xBE, 0xC5, 0xCC,
    0xD3, 0xDA, 0xE0, 0xE5, 0xEA, 0xEF, 0xF3, 0xF6, 0xF9, 0xFC, 0xFD, 0xFE, 0xFF, 0xFE, 0xFD, 0xFC, 0xF9, 0xF6, 0xF3, 0xEF, 0xEA, 0xE5, 0xE0, 0xDA, 0xD3, 0xCC,
    0xC5, 0xBE, 0xB7, 0xAF, 0xA7, 0x9F, 0x98, 0x90, 0x88, 0x80, 0x79, 0x71, 0x6A, 0x63, 0x5C, 0x56, 0x4F, 0x49, 0x43, 0x3E, 0x39, 0x34, 0x2F, 0x2B, 0x27, 0x23,
    0x20, 0x1D, 0x1A, 0x17, 0x14, 0x12, 0x10, 0x0E, 0x0D, 0x0B, 0x0A, 0x08, 0x07, 0x06, 0x05, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01,

    0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0D, 0x0E, 0x10, 0x12, 0x14, 0x17, 0x1A, 0x1D, 0x20, 0x23,
    0x27, 0x2B, 0x2F, 0x34, 0x39, 0x3E, 0x43, 0x49, 0x4F, 0x56, 0x5C, 0x63, 0x6A, 0x71, 0x79, 0x80, 0x88, 0x90, 0x98, 0x9F, 0xA7, 0xAF, 0xB7, 0xBE, 0xC5, 0xCC,
    0xD3, 0xDA, 0xE0, 0xE5, 0xEA, 0xEF, 0xF3, 0xF6, 0xF9, 0xFC, 0xFD, 0xFE, 0xFF, 0xFE, 0xFD, 0xFC, 0xF9, 0xF6, 0xF3, 0xEF, 0xEA, 0xE5, 0xE0, 0xDA, 0xD3, 0xCC,
    0xC5, 0xBE, 0xB7, 0xAF, 0xA7, 0x9F, 0x98, 0x90, 0x88, 0x80, 0x79, 0x71, 0x6A, 0x63, 0x5C, 0x56, 0x4F, 0x49, 0x43, 0x3E, 0x39, 0x34, 0x2F, 0x2B, 0x27, 0x23,
    0x20, 0x1D, 0x1A, 0x17, 0x14, 0x12, 0x10, 0x0E, 0x0D, 0x0B, 0x0A, 0x08, 0x07, 0x06, 0x05, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01,
};

/// Init Slope: Gaussian slope from dark to background brightness
const uint8_t gau8_initSlope[cu16_TemplateLength * 2] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
    0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x08, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x12, 0x13, 0x14, 0x16,
    0x17, 0x19, 0x1A, 0x1C, 0x1D, 0x1F, 0x21, 0x22, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2D, 0x2E, 0x30, 0x32, 0x33, 0x35, 0x37, 0x38, 0x3A, 0x3B, 0x3D, 0x3E, 0x40,
    0x41, 0x42, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4C, 0x4D, 0x4E, 0x4E, 0x4F, 0x4F, 0x50, 0x50, 0x51, 0x51, 0x52, 0x52, 0x52, 0x52, 0x53,
    0x53, 0x53, 0x53, 0x53, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x55,
};

/// Offset Pulse: Gaussian slope from background illumination to full scale brightness
const uint8_t gau8_offsetSlope[cu16_TemplateLength] = {
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58, 0x58, 0x59,
    0x59, 0x5A, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x65, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76, 0x79, 0x7B, 0x7E, 0x81,
    0x84, 0x87, 0x8A, 0x8D, 0x90, 0x93, 0x97, 0x9A, 0x9D, 0xA1, 0xA4, 0xA8, 0xAB, 0xAF, 0xB2, 0xB6, 0xB9, 0xBC, 0xC0, 0xC3, 0xC6, 0xC9, 0xCC, 0xCF, 0xD2, 0xD5,
    0xD8, 0xDA, 0xDD, 0xDF, 0xE1, 0xE3, 0xE5, 0xE7, 0xE9, 0xEB, 0xED, 0xEE, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xF9, 0xFA, 0xFA, 0xFB,
    0xFB, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF,
};

/// Full Slope: Gaussian slope from dark to full scale brightness
const uint8_t gau8_fullSlope[cu16_TemplateLength] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x06,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0D, 0x0F, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1D, 0x1F, 0x22, 0x25, 0x28, 0x2B, 0x2F, 0x32, 0x36, 0x3A, 0x3E, 0x42,
    0x46, 0x4B, 0x50, 0x54, 0x59, 0x5E, 0x63, 0x68, 0x6D, 0x72, 0x77, 0x7C, 0x82, 0x87, 0x8C, 0x91, 0x96, 0x9B, 0xA0, 0xA5, 0xAA, 0xAE, 0xB3, 0xB8, 0xBC, 0xC0,
    0xC4, 0xC8, 0xCC, 0xCF, 0xD3, 0xD6, 0xD9, 0xDC, 0xDF, 0xE1, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xEF, 0xF1, 0xF2, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF8, 0xF9,
    0xFA, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF,
};