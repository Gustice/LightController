/**
 * @file EffectStateMachine.h
 * @author Gustice
 * @brief Effect-State-Machine-Class for processing Effect Macros
 * @version 0.6
 * @date 2019-10-01
 *
 * @copyright Copyright (c) 2019
 *
 */
#pragma once

#include "Basics.h"
#include "Color.h"
#include <stdint.h>

namespace Effect {

/**
 * @brief Effect State Machine Class
 * @details Processes Effect macro array Macro entry subsequently
 * Processing is executed in following manner:
 *  \li Index-incrementing each Tick
 *      \li ticks until duration \ref EffMacro is finished
 *      \li each tick the index incremented by templateLength/duration
 *  \li Repeats-processing
 *      \li Each repeat the tick and the index are set back to start
 *      \li Switches to next Macro enty
 *      \li Next entry is given by 'next' index
 *      \li Color changes can be executed optionally
 */
class EffectSM {
  public:
    /**
     * @brief Construct a new Effect State Machine object
     * @param templateLength Length of waveforms that are used to display
     */
    EffectSM(uint16_t const templateLength) : EffectSM(templateLength, 0, 0){};

    /**
     * @brief Destroy the Effect State Machine
     */
    ~EffectSM();

    /**
     * @brief Construct a new Effect State Machine object
     *
     * @param templateLength Length of waveforms that are used to display
     * @param intensity Idle intensity
     * @param crossFade @todo not implemented yet
     */
    EffectSM(uint16_t const templateLength, uint8_t const intensity, uint8_t const crossFade);
    /// @todo Configuration could be given by structure

    /**
     * @brief Sets the effect state machine to process given effect macro
     * @param sequence reference to sequence
     * @param startColor Start color. Use noColor to invoke with default color
     * @param initialDelay Number of steps that are waited before effect is started
     */
    void SetEffect(Macro_t *sequence, Color_t const *startColor = noColor, uint8_t initialDelay = 0);

    /**
     * @brief Sets the effect state machine to process given effect macro
     * @param sequence reference to sequence
     * @param startColor Start color. Use noColor to invoke with default color
     * @param intens Idle intensity for effect. Use nullptr to start with default intensity
     * @param delayedStart Number of steps that are waited before effect is started
     */
    void SetEffect(Macro_t *sequence, Color_t const *startColor, const uint8_t *intens,
                   const uint8_t delayedStart);

    /**
     * @brief Execute step of state machine
     * @details Each tick the tick variable is decremented. After the limit is reached,
     *  the current macro line is either repeated or the next macro line is started (with
     *  an optional color change).\n
     * If the color is changed by switching the macro line, a dissolve counterr is concurrently
     *  triggered (see \ref GetDissolveRatio). This counter can be used to cross fade between
     *  different colors.
     * @note Tick must be called regularly. For standard light applications all 40 ms seems to be
     *  a convenient values.
     * @return Color const* Reference to color result
     */
    virtual Color const *Tick(void);

    /**
     * @brief Returns fading ramp in cases the color were switched
     * @return uint8_t
     */
    uint8_t GetDissolveRatio(void);

    /**
     * @brief Set the Dynamic Range of effects
     * @param range Dynamic range 0-255. Which applied around idle brightness
     */
    void SetDynamicRange(uint8_t range) { SMIParams.dynamicRange = range; };

    /**
     * @brief Get the index for the current waveform
     * @details The index is calculated with a higher accuracy in the background.
     * @return const uint8_t index to waveform position.
     */
    uint8_t GetWaveIdx(void) { return ((SMPValues.waveIdx & 0xFF00u) >> 8); };

    /**
     * @brief Get current color
     * @return const Color
     */
    /// @todo delete?
    const Color GetColor(void) { return _curentColor; };

    /**
     * @brief Get set intensity
     * @return const uint8_t intensity
     */
    /// @todo delete?
    uint8_t GetIntensity(void) { return SMIParams.idleIntens; };

    /**
     * @brief Get index to current macro in sequence
     * @return EffMacro_type const* const
     */
    Macro_t *const GetStep(void) { return _p_effMac; };

    /**
     * @brief Get Process values of state machen
     *
     * @return const SM_ProcessValues_t
     */
    const SM_ProcessValues_t GetProcessValues(void) { return SMPValues; };

    /// @todo change to protected and tests results
  protected:
    /// First element of Effect macro stack
    Macro_t const *_p_effSeq;
    /// Currently indexed Effect-part
    Macro_t const *_p_effMac;

    /// Reference to output color
    Color *_outputColor; /// @todo !!

    // Color& (*apF_Effects)(EffectSM * SM);
    typedef Color const *pEffPrc(EffectSM *);

    /// Reference to Effect-Color-Processors
    static pEffPrc *const apF_Processors[7];

    /// Black out lights
    static Color const *UpdateBlank(EffectSM *SM);
    /// Continuous illumination in idle intensity
    static Color const *UpdateIdle(EffectSM *SM);
    /// Keep latest state on hold
    static Color const *UpdateFreeze(EffectSM *SM);
    /// Step through a defined waveform
    static Color const *UpdateWave(EffectSM *SM);
    /// Step through a defined waveform backwards
    static Color const *UpdateRevWave(EffectSM *SM);
    /// Show flicker effect with idle intensity as static part with dynamic flicker-range
    static Color const *UpdateFlicker(EffectSM *SM);

    /// Last color
    Color _lastColor;

    /// Current color
    /// \li Either forced by start with parameter
    /// \li Or given by current Effect-part
    Color _curentColor;

    /// Concentrated parameter values of instance
    SM_ParameterValues_t SMIParams;

    /// Concentrated process values of instance
    SM_ProcessValues_t SMPValues;

    /// @todo callback for finished to exclude these indexes

    /**
     * @brief Sets Waveform index according du desired duration, waveform-length and current step
     */
    void SetIndexes(void);

    /// Constructor for inheriting class
    EffectSM(void){};
};

} // namespace Effect