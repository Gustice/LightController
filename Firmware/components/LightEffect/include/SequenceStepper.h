/**
 * @file SequenceStepper.h
 * @author Gustice
 * @brief Effect-State-Machine-Class for processing Effect Macros
 * @version 0.6
 * @date 2021-03-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include "Basics.h"
#include "Color.h"
#include <stdint.h>

namespace Effect {

/**
 * @brief Effect Sequencer State Machines Class
 * @details Maintains State-Machines, Processes effect-switching, and invokes generation
 *  of raw data stream for LEDs.
 *      \li If effect scenarios are changed a crossfading is executed
 *      \li Rawdatastream for each Color object is generated
 */
class SequenceSM {
  public:
    /**
     * @brief Construct a new Effect State Machine object
     * @param templateLength Length of waveforms that are used to display
     * @param targetCount Count of target LEDs
     */
    SequenceSM(uint16_t const templateLength, uint8_t targetCount) : SequenceSM(templateLength, targetCount, 0, 0){};

    /**
     * @brief Destroy the Effect State Machine
     */
    ~SequenceSM();

    /**
     * @brief Construct a new Effect State Machine object
     * @param templateLength Length of waveforms that are used to display
     * @param targetCount Count of target LEDs
     * @param intensity Idle intensity for effect. Use nullptr to start with default intensity
     * @param crossFade Steps count for cross fade
     */
    SequenceSM(uint16_t const templateLength, uint8_t targetCount, uint8_t const intensity, uint8_t const crossFade);
    /// @todo Configuration could be given by structure

    /**
     * @brief Set the Effect object
     *
     * @param sequence reference to sequence that has to be processed next
     * @param startColor Start color. Use noColor to start with default color
     * @param initialDelay Number of steps that are waited before effect is started
     */
    void SetEffect(const Sequence *sequence, Color::color_t const *startColor = noColor, uint8_t initialDelay = 0);

    /**
     * @brief Set the Effect object
     *
     * @param sequence reference to sequence that has to be processed next
     * @param startColor Start color. Use noColor to start with default color
     * @param intens Idle intensity for effect. Use NULL to start with default intensity
     * @param delayedStart Number of steps that are waited before effect is started
     */
    void SetEffect(const Sequence *sequence, Color::color_t const *startColor, const uint8_t *intens,
                   const uint8_t delayedStart);

    /**
     * @brief Execute step of state machine
     * @details Each tick the tick variable is decremented. After the limit is reached,
     *  the current macro line is either repeated or the next macro line is started (with
     *  an optional color change).\n
     * If the color is changed by switching the macro line, a dissolve counterr is concurrently
     *  triggered (see \ref GetDissolveRatio). This counter can be used to cross fade between
     *  different colors.
     *
     * @note Tick must be called regularly. For standard light applications all 40 ms seems to be
     *  a convenient values.
     *
     * @return Color * Reference to color container
     */
    virtual Color *Tick(void);

    /**
     * @brief Get the Dissolve for intermediate crossfade between Effect parts
     * @return uint8_t current scaling
     */
    uint8_t GetDissolveRatio(void);

    /// Length of waveforms that are used to display
    const size_t _templateLength;

    /// Count of target LEDs
    const size_t _targetCount;

    /**
     * @brief Set the Dynamic Range of effects
     * @param range Dynamic range 0-255. Which applied around idle brightness
     */
    /// @todo Fill this with some magic
    void SetDynamicRange(uint8_t range) { SMIParams.dynamicRange = range; };

    /**
     * @brief Get the index for the current waveform
     * @details The index is calculated with a higher accuracy in the background.
     * @return const uint8_t index to waveform position.
     */
    const uint8_t GetWaveIdx(void) { return ((SMPValues.waveIdx & 0xFF00u) >> 8); };

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
    const uint8_t GetIntensity(void) { return SMIParams.idleIntens; };

    /**
     * @brief Get index to current macro in sequence
     * @return EffMacro_type const* const
     */
    Sequence const *const GetStep(void) { return _p_effMac; };

    /**
     * @brief Get Process values of state machen
     *
     * @return const SM_ProcessValues_t
     */
    const SM_ProcessValues_t GetProcessValues(void) { return SMPValues; };

    /// @todo change to protected and tests results
  protected:
    /// First element of Effect macro stack
    Sequence const *_p_effSeq;
    /// Currently indexed Effect-part
    Sequence const *_p_effMac;

    /// Reference to calculated color
    Color *_outputColor; /// @todo !!

    /// Execute Blank-Effect
    void UpdateBlank();
    /// Execute Idle-Effect
    void UpdateIdle();
    /// Execute Freeze-Effect
    void UpdateFreeze();
    /// Execute Wave-Effect
    void UpdateWave();
    /// Execute ReverseWave-Effect
    void UpdateRevWave();
    /// Execute Flicker-Effect
    void UpdateFlicker();

    /// Apply calculated color to all Elements
    void ApplyColorToAllElements(Color &color);

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

    void SetIndexes(void);
};

/// Sparkle effect. Similar to flicker but with low key nose with fewer spikes
Color *LightSparkleSequence(SequenceSM *obj, Color *colors, size_t len);

/// Spipe Effect for tow LEDs. One LED is moved forward. The other is moved backwards
Color *LightSwipeSequence(SequenceSM *obj, Color *colors, size_t len);

/// Rotating a wave accross Targets
Color *LightWaveSequence(SequenceSM *obj, Color *colors, size_t len);

} // namespace Effect