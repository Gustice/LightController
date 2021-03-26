/**
 * @file EffectProcessor.h
 * @author Gustice
 * @brief Effect-Processor-Class for processing effect scenarios and switching actions
 * @version 0.6
 * @date 2019-10-03
 *
 * @copyright Copyright (c) 2019
 *
 */
#pragma once

#include "Color.h"
#include "StateMachine.h"
#include <stdint.h>

namespace Effect {

extern const uint8_t gu8_idleIntensity; // @todo this is 11

/**
 * @brief EffectProcessor Class
 * @details Maintains State-Machines, Processes effect-switching, and invokes generation
 *  of raw data stream for LEDs.
 *      \li If effect scenarios are changed a crossfading is executed
 *      \li Rawdatastream for each Color object is generated
 */
class EffectProcessor {
  public:
    /**
     * @brief Constructor
     * @param templateLength Length of waveform template
     * @param fadeSteps Steps count for cross fade
     */
    EffectProcessor(uint16_t const templateLength, uint8_t const fadeSteps);

    /**
     * @brief Destroy the Effect Processor object
     */
    ~EffectProcessor();

    /**
     * @brief Set next effect sequence
     * @param sequence reference to sequence that has to be processed next
     * @param sColor Start color. Use noColor to start with default color
     * @param intens Idle intensity for effect. Use NULL to start with default intensity
     */
    void SetEffect(Macro_t *sequence, Color::color_t const *sColor = noColor, uint8_t intens = gu8_idleIntensity);

    /**
     * @brief Execute step of effect processor
     * @return Reference to color result
     */
    Color const *Tick(void);

  private:
    /// Target count
    uint8_t _colorSize;
    /// Current color
    Color _pColor;
    /// Last applied color (used to crossfade in intermediate states)
    Color _pColorOld;
    /// Defines length of cross-fading
    uint8_t _fadeSteps;
    /// Current count of cross-fading effect
    uint8_t _fadingCnt;
    /// Processing of current effect
    EffectSM *_EffPV;
    /// Processing of last effect (Last effect ist shortly kept alive to conduct cross fading)
    EffectSM *_EffPV_old;

    /**
     * @brief Cross-fade current and last effect
     * @param k scaling factor for current cross-fade
     * @return Color* Reference to color result
     */
    Color *crossFadeColors(uint8_t k);

    /// Constructor for inherited classes
    EffectProcessor();
};

} // namespace Effect