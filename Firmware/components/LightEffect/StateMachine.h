#pragma once

#include <stdint.h>
#include "Color.h"
#include "Macro.h"

/**
 * @brief Effect State Machine Class
 * @details Processes Effect macro array Macro entry subsequently
 * Processing is executed in following manner:
 *  \li Index-incrementing each Tick
 *      \li ticks until duration \ref EffMacro is finished
 *      \li each tick the index incremented by templateLenght/duration
 *  \li Repeats-processing
 *      \li Each repeat the tick and the index are set back to start
 *      \li Switches to next Macro enty
 *      \li Next entry is given by 'next' index
 *      \li Color changes can be executed optionally
 */
class EffectSM {
  public:
    EffectSM(uint16_t const templateLength) : EffectSM(templateLength, 0, 0) {};
    EffectSM(uint16_t const templateLength, uint8_t const intensity, uint8_t const crossFade);
    
    void    SetEffect(EffMacro_type *sequence, Color_t const *startColor = NO_COLOR, uint8_t intialDelay = 0);
    void    SetEffect(EffMacro_type *sequence, Color_t const *startColor, uint8_t delayedStart, uint8_t intens);
    uint8_t Tick(void);
    uint8_t GetDissolveRatio(void);
    
    uint8_t GetWaveformIdx(void) {return (_u16_waveIdx >> 8); };
    Color GetColor(void) {return _curentColor;};
    uint8_t GetIntensity(void) {return _u8_idleIntens;};

    EffMacro_type * GetStep(void) { return _p_effMac; };
    

  private: // @todo change to protected and tests results
    /// First element of Effect macro stack
    EffMacro_type const *_p_effSeq;
    /// Currently indexed Effect-part
    EffMacro_type const *_p_effMac;

    /// Decrementing tick, counting duration
    uint8_t _u8_tick;
    /// Decrementing repeats of current Effect-part
    uint8_t  _u8_repeats;
    /// Dissolve counter dissolves the leading color each tick
    uint8_t _u8_dissolveCnt;

/// @todo Configuration could be given by structure
    /// Current color 
    /// \li Either forced by start with parameter
    /// \li Or given by current Effect-part
    Color   _curentColor;
    /// Idle intensity as background color
    uint8_t _u8_idleIntens;
    /// Fade steps for color changes
    uint8_t _u8_fadeSteps;
    /// Waveform template length -> index steps are calculated accordingly by give duration 
    uint16_t _u16_templateLength;
    /// Current index to waveform
    uint16_t _u16_waveIdx;
    /// Index step each tick according to duration and template length.
    uint16_t _u16_waveStep;

/// @todo callback vor finished to exclude these indexes

    void    SetIndexes(void);
};
