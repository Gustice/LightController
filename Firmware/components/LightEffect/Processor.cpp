/**
 * @file Processor.cpp
 * @author Gustice
 * @brief Implementation of Effect-Processor-Class EffectProcessor.h
 * @version 0.6
 * @date 2019-10-03
 *
 * @copyright Copyright (c) 2019
 */

#include "Processor.h"
#include "Basics.h"
#include "Macro.h"
#include "Waveforms.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace Effect {

EffectProcessor::EffectProcessor(uint16_t const templateLength, uint8_t const fadeSteps)
    : _pColor(), _pColorOld() {
    _EffPV = new EffectSM(templateLength); 
    _EffPV_old = new EffectSM(templateLength);
    _EffPV->SetEffect(macDark, 0);
    _fadeSteps = fadeSteps;
}

EffectProcessor::~EffectProcessor(){
    delete _EffPV;
    delete _EffPV_old;
}

void EffectProcessor::SetEffect(Macro_t *sequence, Color::color_t const *sColor, uint8_t intens) {
    _fadingCnt = _fadeSteps;

    EffectSM * pEsm = _EffPV_old;
    _EffPV_old = _EffPV;
    _EffPV = pEsm;
    _EffPV->SetEffect(sequence, sColor, &intens, 0);
}

Color const * EffectProcessor::Tick(void) {
    uint8_t k;
    _pColor = _EffPV->Tick();

    if (_fadingCnt > 0) { // allways process soft cross dissolve between different macros
        _fadingCnt--;
        k = (uint16_t)_fadingCnt * 0xFF / _fadeSteps;

        _pColorOld = _EffPV_old->Tick();
        crossFadeColors(k);
    }

    return &_pColor;
}

Color *EffectProcessor::crossFadeColors(uint8_t k) {
    _pColorOld = _pColorOld * k;
    _pColor    = _pColor * (0xFF - k);
    _pColor    = _pColor + _pColorOld;

    return &_pColor;
}


} // namespace Effect