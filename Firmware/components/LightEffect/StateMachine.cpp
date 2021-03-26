/**
 * @file StateMachine.cpp
 * @author Gustice
 * @brief Implementation of Effect-State-Machine-Class EffectStateMachine.h
 * @version 0.6
 * @date 2019-10-01
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "StateMachine.h"
#include "Waveforms.h"
#include <math.h>
#include <stdlib.h>

namespace Effect {

/// Optional prequel for delayed start.
/// \li Duration must be != 0
/// \li Next entry will be first entry in given Effect sequence
Macro_t delayPrequel[] = {
    {Light_Blank, (uint8_t *)0, 0, 1, &color_Black, 0, 0},
};

/// Function table to processing Functions
EffectSM::pEffPrc *const EffectSM::apF_Processors[7] = {
    EffectSM::UpdateBlank, EffectSM::UpdateIdle,    EffectSM::UpdateFreeze,
    EffectSM::UpdateWave,  EffectSM::UpdateRevWave, EffectSM::UpdateFlicker, 
    EffectSM::UpdateFreeze // @todo custom effect not in use here
};

EffectSM::EffectSM(uint16_t const templateLength, uint8_t const intensity, uint8_t const crossFade) {
    SMIParams.templateLength = templateLength;
    SMIParams.idleIntens     = intensity;
    SMIParams.fadeSteps      = 0;
    SMIParams.dynamicRange   = 30;
    if (crossFade > 0)
        SMIParams.fadeSteps = crossFade; // @todo
    _outputColor = new Color;
}

EffectSM::~EffectSM() { delete _outputColor; }

void EffectSM::SetEffect(Macro_t *sequence, Color_t const *startColor, uint8_t initialDelay) {
    SetEffect(sequence, startColor, &SMIParams.idleIntens, initialDelay);
}

/**
 * @brief 
 *
 * @param sequence
 * @param startColor
 * @param delayedStart
 * @param intens
 */
void EffectSM::SetEffect(Macro_t *sequence, Color_t const *startColor, const uint8_t *intens,
                         const uint8_t delayedStart) {
    if (startColor != noColor) {
        _curentColor.SetColor(*startColor);
    } else {
        _curentColor.SetColor(*sequence->pColor);
    }
    _lastColor.SetColor(_curentColor.GetColor());
    if (intens != nullptr) {
        SMIParams.idleIntens = *intens;
    }

    SMPValues.macroIdx = 0;

    _p_effSeq = sequence;
    if (delayedStart > 0) {
        _p_effMac      = delayPrequel;
        SMPValues.tick = delayedStart;
    } else {
        _p_effMac      = sequence;
        SMPValues.tick = _p_effMac->duration;
    }

    SMPValues.tick++;
    SMPValues.dissolveCnt = 0;
    SMPValues.repeats     = _p_effMac->repeats;
    this->SetIndexes();
}

Color const *EffectSM::Tick(void) {
    // tick-increment
    if (--SMPValues.tick == 0) {
        // repeats-decrement
        if (SMPValues.repeats-- == 0) {
            SMPValues.macroIdx = _p_effMac->next;
            _p_effMac          = &(_p_effSeq[_p_effMac->next]);
            SMPValues.repeats  = _p_effMac->repeats;

            // execute color change if necessary
            if (_p_effMac->pColor != noColor) {
                SMPValues.dissolveCnt = SMIParams.fadeSteps;
                _lastColor.SetColor(_curentColor.GetColor());
                _curentColor.SetColor(*_p_effMac->pColor);
            }
        }

        SMPValues.tick = _p_effMac->duration;
        this->SetIndexes();
    }

    SMPValues.waveIdx += SMPValues.waveStep;
    if (SMPValues.dissolveCnt > 0) {
        SMPValues.dissolveCnt--;
    }

    return apF_Processors[_p_effMac->state](this);
}

void EffectSM::SetIndexes(void) {
    SMPValues.waveStep = ((SMIParams.templateLength) << 8);
    SMPValues.waveStep /= _p_effMac->duration;
    SMPValues.waveIdx = (uint16_t)(0 - SMPValues.waveStep);
}

/// @todo it seems not te be very clever to delegate this on higher level
uint8_t EffectSM::GetDissolveRatio(void) {
    if (SMIParams.fadeSteps == 0)
        return 0;

    uint8_t dissolving = (uint8_t)((uint16_t)0xFF * SMPValues.dissolveCnt / SMIParams.fadeSteps);
    return dissolving;
}

Color const *EffectSM::UpdateBlank(EffectSM *SM) {
    *(SM->_outputColor) = SM->_curentColor * 0;
    return SM->_outputColor;
}

Color const *EffectSM::UpdateIdle(EffectSM *SM) {
    *(SM->_outputColor) = SM->_curentColor * SM->SMIParams.idleIntens;
    return SM->_outputColor;
}

Color const *EffectSM::UpdateFreeze(EffectSM *SM) {
    // just do nothing and Return currently set color
    return SM->_outputColor;
}

Color const *EffectSM::UpdateWave(EffectSM *SM) {
    Macro_t const *const cEffStep = SM->GetStep();
    *(SM->_outputColor)           = SM->GetColor() * cEffStep->pWave[SM->GetWaveIdx()] * cEffStep->FsIntensity;
    return SM->_outputColor;
}

Color const *EffectSM::UpdateRevWave(EffectSM *SM) {
    Macro_t const *const cEffStep  = SM->GetStep();
    uint8_t              lastIndex = SM->SMIParams.templateLength - 1;
    *(SM->_outputColor) = SM->GetColor() * cEffStep->pWave[lastIndex - SM->GetWaveIdx()] * cEffStep->FsIntensity;
    return SM->_outputColor;
}

Color const *EffectSM::UpdateFlicker(EffectSM *SM) {
    uint8_t k;
    k                   = rand();
    k                   = SM->SMIParams.idleIntens + ((int16_t)k * (SM->SMIParams.dynamicRange) / 0xFF);
    *(SM->_outputColor) = SM->_curentColor * k;
    return SM->_outputColor;
}

} // namespace Effect