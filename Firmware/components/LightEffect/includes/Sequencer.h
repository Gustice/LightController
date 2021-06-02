/**
 * @file EffectSequencer.h
 * @author Gustice
 * @brief Effect-Processor-Class for processing effect scenarios and switching actions
 * @version 0.6
 * @date 2021-03-12
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include "Color.h"
#include "SequenceStepper.h"
#include <stdint.h>

namespace Effect {

/**
 * @brief EffectSequencer Class
 * @details Maintains State-Machines, Processes effect-switching, and invokes generation
 *  of raw data stream for LEDs.
 *      \li If effect scenarios are changed a crossfading is executed
 *      \li Rawdatastream for each Color object is generated
 */
class EffectSequencer {
  public:
    /**
     * @brief Construct a new Effect Sequencer object
     * @param templateLength Length of waveform template
     * @param targetCount Count of LEDs to maintain
     * @param fadeSteps Steps count for cross fade
     */
    EffectSequencer(uint16_t const templateLength, uint8_t targetCount, uint8_t const fadeSteps);

    /**
     * @brief Destroy the Effect Sequencer object
     */
    ~EffectSequencer();

    /**
     * @brief Set the Effect sequence
     * @param sequence reference to sequence that has to be processed next
     * @param sColor Start color. Use noColor to start with default color
     * @param intens Idle intensity for effect. Use NULL to start with default intensity
     */
    void SetEffect(const Sequence *sequence, Color_t const *sColor = noColor, uint8_t intens = gu8_idleIntensity, uint8_t delay = 0);


    /**
     * @brief Get the currently active Color. 
     * @details In transient conditions during fading, only the new color is outputted.
     * @return Color_t Currently active color
     */
    Color_t GetActiveColor(void) {return cColor.GetColor();};

    /**
     * @brief Execute step of effect sequencer
     * @return Reference to color result
     */
    Color const *Tick(void);

  private:
    /// Currently active Color
    Color cColor;
    /// Current color image (after Tick)
    Color *_pColor;
    /// Last applied color (used to crossfade in intermediate states)
    Color *_pColorOld;
    /// Defines length of cross-fading
    uint8_t _fadeSteps;
    /// Current count of cross-fading effect
    uint8_t _fadingCnt;
    /// Target count
    const uint8_t _targetCount;
    /// Processing of current effect
    SequenceSM *_EffPV;
    /// Processing of last effect (Last effect ist shortly kept alive to conduct cross fading)
    SequenceSM *_EffPV_old;

    /**
     * @brief Cross-fade current and last effect
     * @param k scaling factor for current cross-fade
     * @return Color* Reference to color result
     */
    Color *crossFadeColors(uint8_t k);

    /// Constructor for inherited classes
    EffectSequencer();
};

} // namespace Effect