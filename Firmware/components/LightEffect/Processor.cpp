#include "Processor.h"
#include "Waveforms.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

EffectProcessor::EffectProcessor(uint16_t const templateLength, uint8_t const fadeSteps)  
      : EffPV(templateLength), 
        EffPV_old(templateLength), 
        _pColor(), 
        _pColorOld() {
    EffPV.SetEffect(eff_Dark, 0);
    u8_fadeSteps     = fadeSteps;
    u8_dynamicRange = 30;
}

void EffectProcessor::SetEffect(EffMacro_type *sequence, Color_t const *sColor, uint8_t intens) {
    u8_fadingCnt = u8_fadeSteps;

    memcpy(&EffPV_old, &EffPV, sizeof(EffPV));
    EffPV.SetEffect(sequence, sColor, 0, intens);
}

void EffectProcessor::Tick(void) {
    uint8_t k;
    EffPV.Tick();
    GenerateImage(&_pColor, &EffPV);

    if (u8_fadingCnt > 0) { // allways process soft cross dissolve between different macros
        u8_fadingCnt--;
        k = (uint16_t)u8_fadingCnt * 0xFF / u8_fadeSteps;

        EffPV_old.Tick();
        GenerateImage(&_pColorOld, &EffPV_old);
        crossFadeColors(k);
    } else { // Process soft cross dissolve between different colors, if needed
        uint8_t k = EffPV.GetDissolveRatio();
        if (k > 0) {
            crossFadeColors(k);
        }
        memcpy(&_pColorOld, &_pColor, sizeof(Color));
    }
}

Color *EffectProcessor::crossFadeColors(uint8_t k) {
    // int i;
    // for (i = 0; i < _colorSize; i++) {
    //     _pColorOld[i] = _pColorOld[i] * k;
    //     _pColor[i]    = _pColor[i] * (0xFF - k);
    //     _pColor[i]    = _pColor[i] + _pColorOld[i];
    // }

        _pColorOld = _pColorOld * k;
        _pColor    = _pColor * (0xFF - k);
        _pColor    = _pColor + _pColorOld;

    return &_pColor;
}

void EffectProcessor::GenerateImage(Color *color, EffectSM *effStat) {
    uint8_t k, kr;

    EffMacro_type const *const cEffStep = effStat->GetStep();
    switch (cEffStep->state) {
    case Light_Idle:
        *color = effStat->GetColor() * effStat->GetIntensity();
        break;

    case Light_Wave:
        *color = effStat->GetColor() * cEffStep->pu8_wave[effStat->GetWaveformIdx()] * cEffStep->u8_FSintensity;
        break;

    case Light_InvWave:
        *color = effStat->GetColor() * cEffStep->pu8_wave[cu16_TemplateLength - 1 - effStat->GetWaveformIdx()] * cEffStep->u8_FSintensity;
        break;

    case Light_Flicker:
        k      = rand();
        k      = 50 + ((int16_t)k * (50 + u8_dynamicRange) / 0xFF);
        *color = effStat->GetColor() * k;
        break;

    case Light_Sparkle:
        kr     = ((int16_t)rand() * 14 / 0xFF);
        k      = 50 + kr * kr;
        *color = effStat->GetColor() * k;
        break;

    case Light_Blank:
        *color = effStat->GetColor() * 0;
        break;

    case Light_Freeze:

        break;

    default:

        break;
    }
}

// /************************************************************************/
// /* EffectProcessor 1 LED                                                         */
// /************************************************************************/
// SingleEffect::SingleEffect(uint16_t const templateLength) :
//     EffectProcessor(templateLength) {
// 	_colorSize = 1;
// 	_pColor = _Color;
// 	_pColorOld = _ColorOld;
// }

// void SingleEffect::genImage(Color * color, EffectSM * effStat) {
// 	uint8_t k, kr;

//     // EffMacro_type const * const cEffStep = effStat->GetStep();
// 	// switch (cEffStep->state)
// 	// {
// 	// 	case Light_Idle:
// 	// 	*color = effStat->_aColor * effStat->_idleIntens;
// 	// 	break;

// 	// 	case Light_Wave:
// 	// 	*color = effStat->_aColor * cEffStep->pu8_wave[effStat->ReadTempIdx()] * cEffStep->u8_FSintensity;
// 	// 	break;

// 	// 	case Light_InvWave:
// 	// 	*color = effStat->_aColor * cEffStep->pu8_wave[cu16_TemplateLength -1 - effStat->ReadTempIdx()] * cEffStep->u8_FSintensity;
// 	// 	break;

// 	// 	case Light_Flicker:
// 	// 	k = rand();
// 	// 	k =  50 + ((int16_t) k * (50 + _u8_FlickerRange) / 0xFF);
// 	// 	//k = smoothFlick.GetNewAverage(k);
// 	// 	*color = effStat->_aColor * k;
// 	// 	break;

// 	// 	case Light_Sparkle:
// 	// 	kr = ((int16_t)rand() *14 / 0xFF);
// 	// 	k =  50 + kr * kr;
// 	// 	*color = effStat->_aColor * k;
// 	// 	break;

// 	// 	case Light_Blank:
// 	// 	*color = effStat->_aColor * 0;
// 	// 	break;

// 	// 	case Light_Freeze:
// 	// 	default:

// 	// 	break;
// 	// }
// }

// /************************************************************************/
// /* EffectProcessor 2 LED                                                         */
// /************************************************************************/
// DualEffect::DualEffect(uint16_t const templateLength) : EffectProcessor(templateLength) {
// 	_colorSize = 2;
// 	_pColor = _Color;
// 	_pColorOld = _ColorOld;
// }

// void DualEffect::genImage(Color * color, EffectSM * effStat) {
// 	// switch (effStat->_state)
// 	// {
// 	// 	case Light_Idle:
// 	// 	color[0] = color[1] = effStat->_aColor * effStat->_idleIntens;
// 	// 	break;

// 	// 	case Light_Wave:
// 	// 	color[0] = effStat->_aColor * effStat->_p_effMac->pu8_wave[effStat->ReadTempIdx()] * effStat->_p_effMac->u8_FSintensity;
// 	// 	color[1] = effStat->_aColor * effStat->_p_effMac->pu8_wave[cu16_TemplateLength -1 - effStat->ReadTempIdx()] * effStat->_p_effMac->u8_FSintensity;
// 	// 	break;

// 	// 	case Light_InvWave:
// 	// 	color[0] = effStat->_aColor * effStat->_p_effMac->pu8_wave[cu16_TemplateLength -1 - effStat->ReadTempIdx()] * effStat->_p_effMac->u8_FSintensity;
// 	// 	color[1] = effStat->_aColor * effStat->_p_effMac->pu8_wave[effStat->ReadTempIdx()] * effStat->_p_effMac->u8_FSintensity;
// 	// 	break;

// 	// 	case Light_Blank:
// 	// 	color[0] = color[1] = effStat->_aColor * 0;
// 	// 	break;

// 	// 	case Light_Flicker:
// 	// 	case Light_Sparkle:
// 	// 	case Light_Freeze:
// 	// 	default:

// 	// 	break;
// 	// }
// }

/************************************************************************/
/* EffectProcessor xx LEDs                                                       */
/************************************************************************/
// MultiEffect::MultiEffect(uint8_t u8_Length) : EffectProcessor() {
// 	_colorSize = u8_Length;
// 	//  @todo pointer to external ColorSource
// }

// void MultiEffect::genImage(Color * color, EffectSM const * effStat) {
// 	uint8_t i;
// 	// switch (effStat->_state)
// 	// {
// 	// 	case Light_Idle:
// 	// 	{
// 	// 		Color col = effStat->_aColor * effStat->_idleIntens;
// 	// 		for (i=0; i < _colorSize; i++)
// 	// 		{
// 	// 			color[i] = col;
// 	// 		}
// 	// 	}
// 	// 	break;

// 	// 	case Light_Wave:
// 	// 	{
// 	// 		uint8_t cStep = effStat->ReadTempIdx();
// 	// 		uint8_t step = cu16_TemplateLength / _colorSize ;

// 	// 		for (i=0; i < _colorSize; i++)
// 	// 		{
// 	// 			color[i] = effStat->_aColor * effStat->_p_effMac->pu8_wave[cStep] * effStat->_p_effMac->u8_FSintensity;
// 	// 			cStep += step;
// 	// 		}
// 	// 	}
// 	// 	break;

// 	// 	case Light_Blank:
// 	// 	{
// 	// 		Color col = effStat->_aColor * 0;
// 	// 		for (i=0; i < _colorSize; i++)
// 	// 		{
// 	// 			color[i] = col;
// 	// 		}
// 	// 	}
// 	// 	break;

// 	// 	case Light_Flicker:
// 	// 	case Light_Sparkle:
// 	// 	case Light_Freeze:
// 	// 	default:

// 	// 	break;
// 	// }
// }
