/**
 * @file EffectComplex.cpp
 * @author Gustice
 * @brief Implementation of Effect Complex
 * @version 0.1
 * @date 2021-06-01
 *
 * @copyright Copyright (c) 2021
 */

#include "EffectComplex.h"

/// Simple effect macro that starts a set color smoothly and then holds its state
const Effect::Sequence StartupLightSequence[] = {
    Effect::Sequence(16, 1, Effect::eEffect::Light_Blank),
    Effect::Sequence(64, 2, Effect::gau8_initSlope, Effect::eEffect::Light_Wave),
    Effect::Sequence(64, 2, Effect::eEffect::Light_Idle), // Infinit Loop
};

void EffectComplex::ActivateStartupEffects() {
    for (size_t i = 0; i < Count; i++) {
        if (pConfig[i].Target != TargetGate::None) {
            Sequencers[i]->SetEffect(
                StartupLightSequence, &pConfig[i].Color, gu8_idleIntensity, pConfig[i].Delay);
        }
    }
}

esp_err_t EffectComplex::FadeToColor(int index, Color_t &color) {
    if (index >= Count)
        return ESP_FAIL;
    Sequencers[index]->SetEffect(StartupLightSequence, &color, gu8_idleIntensity, 0);
    return ESP_OK;
}

esp_err_t EffectComplex::ReadColor(int index, Color_t &color) {
    if (index >= Count)
        return ESP_FAIL;
    Color_t c = Sequencers[index]->GetActiveColor();
    color.red = c.red;
    color.green = c.green;
    color.blue = c.blue;
    color.white = c.white;
    return ESP_OK;
}
