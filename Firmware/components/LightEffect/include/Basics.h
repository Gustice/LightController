/**
 * @file Basics.h
 * @author Gustice
 * @brief Definitions for color effects
 * @version 0.6
 * @date 2021-03-23
 * 
 * @copyright Copyright (c) 2021
 */

#pragma once

#include "Color.h"
#include "Waveforms.h"
#include <stdint.h>

namespace Effect {

/**
 * @brief Effect that is applied to color
 */
enum eEffect {
    /// Black out lights
    Light_Blank = 0,
    /// Continuous illumination in idle intensity
    Light_Idle,
    /// Keep latest state on hold
    Light_Freeze,
    /// Step through a defined waveform
    Light_Wave,
    /// Step through a defined waveform backwards
    Light_RevWave,
    /// Show flicker effect with idle intensity as static part with dynamic flicker-range
    Light_Flicker,
    /// Custom effect, this is provided by Effect-Sequence
    LightCustom,

    Light_States,
};

/**
 * @brief Definition of effect that is processed each step
 */
typedef const struct Macro_def{
    /// Effect type
    eEffect state;
    /// waveform reference
    uint8_t const *pWave;
    /// Full-Scale-intensity
    uint8_t FsIntensity;
    /// Effect duration
    uint8_t duration;
    /// SubEffect color
    const Color_t *pColor;
    /// Num of Repeats
    int8_t repeats;
    /// Next SubEffect
    int8_t next;
} Macro_t;
#define COUNT_EFFECT_ELEMENT(effect) (sizeof(effect) / sizeof(EffMacro_type))

class SequenceSM;
/**
 * @brief Custom Sequence effect to process
 * @param obj Reference to object
 * @param len Target count of sequencer
 * @return typedef const*
 */
typedef Color *(*pEffPrc)(SequenceSM *obj, Color *colors, size_t len);


/**
 * @brief Definition of effect-sequence that is processed each step
 */
class Sequence {
  public:
    /// Effect type
    const eEffect state;
    /// waveform reference
    const uint8_t *const pWave;
    /// Full-Scale-intensity
    const uint8_t FsIntensity;
    /// Effect duration
    const uint8_t duration;
    /// SubEffect color
    Color_t const *const pColor;
    /// Num of Repeats
    const int8_t repeats;
    /// Next SubEffect
    const int8_t next;
    /// Custom processor function
    const pEffPrc pProcessor;

    /**
     * @brief Const-Constructor for simple effects
     */
    constexpr Sequence(uint8_t duration, uint8_t next, eEffect state = eEffect::Light_Freeze,
                          Color_t const *pColor = oldColor)
        : state(state), pWave(nullptr), FsIntensity(gu8_fullIntensity), duration(duration), pColor(pColor), repeats(0),
          next(next), pProcessor(nullptr){}; // @todo try to use noWave for pWave-init

    /**
     * @brief Const-Constructor for waveform effects
     */
    constexpr Sequence(uint8_t duration, uint8_t next, uint8_t const *pWave, eEffect state,
                          Color_t const *pColor = oldColor, uint8_t repeat = 0)
        : state(state), pWave(pWave), FsIntensity(gu8_fullIntensity), duration(duration), pColor(pColor),
          repeats(repeat), next(next), pProcessor(nullptr){};

    /**
     * @brief Const-Constructor for elaborated effects with custom algorithm
     */
    constexpr Sequence(uint8_t duration, uint8_t next, pEffPrc pProcessor, uint8_t const *pWave,
                          Color_t const *pColor = oldColor, uint8_t repeat = 0)
        : state(eEffect::LightCustom), pWave(pWave), FsIntensity(gu8_fullIntensity), duration(duration), pColor(pColor),
          repeats(repeat), next(next), pProcessor(pProcessor){};

    /**
     * @brief Const-Constructor by legacy structure
     */
    constexpr Sequence(Macro_t macro)
        : state(macro.state), pWave(macro.pWave), FsIntensity(macro.FsIntensity), duration(macro.duration),
          pColor(macro.pColor), repeats(macro.repeats), next(macro.next), pProcessor(nullptr){};
};

/// Effect sequence frame
typedef struct {
    /// Element for visualization
    const Macro_t *element; 
    /// Number of registered steps
    uint8_t           content; 
    /// Repetitions
    uint8_t           repeats; 
} EffSequence_t;
#define COUNT_EFFECT_MACRO(effect) (sizeof(effect) / sizeof(EffSequence_type))


/// Collection of process values to maintain processing state machine
typedef struct SM_ProcessValues_def {
    /// Decrementing tick, counting duration
    uint8_t tick;
    /// Decrementing repeats of current Effect-part
    uint8_t repeats;
    /// Dissolve counter dissolves the leading color each tick
    uint8_t dissolveCnt;
    /// Index to macro element
    uint8_t macroIdx;
    /// Current index to waveform
    uint16_t waveIdx;
    /// Index step each tick according to duration and template length.
    uint16_t waveStep;
} SM_ProcessValues_t;

/// Collection of object configuration at statup
typedef struct SM_ParamValues_def {
    /// Idle intensity as background color
    uint8_t idleIntens;
    /// dynamic range as variation around background color
    uint8_t dynamicRange;
    /// Fade steps for color changes
    uint8_t fadeSteps;
    /// Waveform template length -> index steps are calculated accordingly by give duration
    uint16_t templateLength;
} SM_ParameterValues_t;

} // namespace Effect