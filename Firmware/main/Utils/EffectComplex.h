/**
 * @file EffectComplex.h
 * @author Gustice
 * @brief Managing configured effects
 * @version 0.1
 * @date 2021-06-01
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "ChannelIndexes.h"
#include "ParamReader.h"
#include "Sequencer.h"

using namespace Effect;

/**
 * @brief Creates configured number of effect processors and manages them
 */
class EffectComplex {
  private:
    /// Pool of Effect channels that drive groups of pixels smoothly
    EffectSequencer **Sequencers;
    /// Reference to effect configuration
    effectProcessor_t *pConfig;

  public:
    /// Number of effect channels
    const size_t Count;

    /**
     * @brief Construct of effect complex 
     * @param count Number of effect channels
     * @param EffectMachines configuration of effects channels
     */
    EffectComplex(size_t count, effectProcessor_t *EffectMachines) : Count(count) {
        pConfig = EffectMachines;
        Sequencers = new EffectSequencer *[Count];
        for (size_t i = 0; i < Count; i++) {
            Sequencers[i] = new EffectSequencer(Effect::cu16_TemplateLength, 1, 16);
        }
    }

    ~EffectComplex() {
        for (size_t i = 0; i < Count; i++) {
            delete Sequencers[i];
        }
        delete[] Sequencers;
    }

    /**
     * @brief Enables effect channels and starts initial color
     */
    void ActivateStartupEffects();

    /**
     * @brief Change color for channel smoothly
     * @param index Channel index
     * @param color New color
     * @return esp_err_t Returns error if channel not defined
     */
    esp_err_t FadeToColor(int index, Color_t& color);

    /**
     * @brief Read active color setting
     * @param index Channel index
     * @param color Read color
     * @return esp_err_t Returns error if channel not defined
     */
    esp_err_t ReadColor(int index, Color_t& color);

    const Color * TickEffect(int index) {
        return Sequencers[index]->Tick();
    }
};