/**
 * @file EffectMacro.cpp
 * @author Gustice
 * @brief Predefined Macros
 * @version 0.6
 * @date 2021-03-23
 * 
 * @copyright Copyright (c) 2021
 */

#include "Macro.h"
#include "Waveforms.h"

namespace Effect {

// EffMacro_type
// state            wave				FS		duration    color	            repeats     next
Macro_t macDark[] = {
    {Light_Blank    , (uint8_t *)0, 0, 32, &color_Black, 0, 0},
};

Macro_t macIdle[] = {
    {Light_Idle, (uint8_t *)0, 0xFF, 32, noColor, 0, 1},
    {Light_Idle, (uint8_t *)0, 0xFF, 32, oldColor, 0, 1},
};

Macro_t macStartIdle[] = {
    {Light_Wave, gau8_initSlope, 0xFF, 24, &color_ColdWhite, 0, 1},
    {Light_Idle, (uint8_t *)0, 0xFF, 32, oldColor, 0, 1},
};

Macro_t macStartFull[] = {
    {Light_Wave, gau8_fullSlope, 0xFF, 24, &color_ColdWhite, 0, 1},
    {Light_Idle, (uint8_t *)0, 0xFF, 32, oldColor, 0, 1},
};

Macro_t macStdPulse[] = {
    {Light_Idle, (uint8_t *)0, 0, 32, noColor, 0, 1},
    {Light_Wave, gau8_offsetPulse, 0xFF, 32, oldColor, 0, 2},
    {Light_Idle, (uint8_t *)0, 0, 32, oldColor, 0, 0},
};

Macro_t macNervousPulse[] = {
    {Light_Idle, (uint8_t *)0, 0, 8, noColor, 0, 1},
    {Light_Wave, gau8_offsetPulse, 0xFF, 16, oldColor, 0, 2},
    {Light_Idle, (uint8_t *)0, 0, 8, oldColor, 0, 0},
};

Macro_t macStdWipe[] = {
    {Light_Idle, (uint8_t *)0, 0, 32, &color_ColdWhite, 0, 1},

    {Light_Wave, gau8_offsetSlope, 0xFF, 32, oldColor, 0, 2},
    {Light_Freeze, (uint8_t *)0, 0, 32, oldColor, 0, 3},
    {Light_RevWave, gau8_offsetSlope, 0xFF, 32, oldColor, 0, 4},
    {Light_Freeze, (uint8_t *)0, 0, 32, oldColor, 0, 1},
};

const Sequence StdPulse[] = {
    Sequence(32, 1, Light_Idle, &color_White),
    Sequence(32, 2, gau8_offsetPulse, eEffect::Light_Wave),
    Sequence(32, 1) // <- Back to one to not change color
};

const Sequence StdDark[] = {
    Sequence(32, 1, Light_Idle, &color_Black),
};

} // namespace Effect
