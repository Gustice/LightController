/**
 * @file Sequecer.cpp
 * @author Gustice
 * @brief Implementation of Effect-Sequencer-Class EffectSequencer.h
 * @version 0.6
 * @date 2021-03-23
 * 
 * @copyright Copyright (c) 2021
 */

#include "Sequencer.h"
#include "Basics.h"
#include "Macro.h"
#include "Waveforms.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace Effect {

EffectSequencer::EffectSequencer(uint16_t const templateLength, uint8_t targetCount, uint8_t const fadeSteps)
    : _targetCount(targetCount), _pColor(), _pColorOld() {
    _EffPV = new SequenceSM(templateLength, targetCount); 
    _EffPV_old = new SequenceSM(templateLength, targetCount);
    _EffPV->SetEffect(StdDark, 0);
    _fadeSteps = fadeSteps;
}

EffectSequencer::~EffectSequencer(){
    delete _EffPV;
    delete _EffPV_old;
}

void EffectSequencer::SetEffect(Sequence *sequence, Color::color_t const *sColor, uint8_t intens) {
    _fadingCnt = _fadeSteps;

    SequenceSM * pEsm = _EffPV_old;
    _EffPV_old = _EffPV;
    _EffPV = pEsm;
    _EffPV->SetEffect(sequence, sColor, &intens, 0);
}

Color const * EffectSequencer::Tick(void) {
    uint8_t k;
    _pColor = _EffPV->Tick();

    if (_fadingCnt > 0) { // allways process soft cross dissolve between different macros
        _fadingCnt--;
        k = (uint16_t)_fadingCnt * 0xFF / _fadeSteps;

        _pColorOld = _EffPV_old->Tick();
        crossFadeColors(k);
    }

    return _pColor;
}

Color *EffectSequencer::crossFadeColors(uint8_t k) {
    for (int i = 0; i < _targetCount; i++) {
        _pColorOld[i] = _pColorOld[i] * k;
        _pColor[i]    = _pColor[i] * (0xFF - k);
        _pColor[i]    = _pColor[i] + _pColorOld[i];
    }
    return _pColor;
}


} // namespace Effect