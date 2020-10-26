#pragma once

#include "Color.h"
#include "StateMachine.h"
#include <stdint.h>

extern const uint8_t gu8_idleIntensity; // @todo this is bad

/**
 * @brief EffectProcessor Class
 * @details Maintains State-Machines, Processes effect-switching, and invokes generation
 *  of raw data stream for LEDs.
 *      \li If effect scenarios are changed a crossfading is executed
 *      \li Rawdatastream for each Color object is generated
 */
class EffectProcessor {
  public:
    EffectProcessor(uint16_t const templateLength, uint8_t const fadeSteps);
    void SetEffect(EffMacro_type *sequence, Color_t const *sColor = NO_COLOR, uint8_t intens = gu8_idleIntensity);
    void Tick(void);
    void SetDynamicRange(uint8_t range) {u8_dynamicRange = range;};

    EffectSM EffPV;
    EffectSM EffPV_old;

    uint8_t _colorSize;
    Color _pColor;
    Color _pColorOld;

    virtual void GenerateImage(Color *color, EffectSM *effStat);

    uint8_t u8_fadeSteps;
    uint8_t u8_fadingCnt;
    uint8_t u8_dissCnt;

    uint8_t  u8_dynamicRange;

  private:
    Color *crossFadeColors(uint8_t k);
};

// #define SINGLEEFFECT_COLORWITH 1
// class SingleEffect : public EffectProcessor {
// 	public:
// 	SingleEffect(uint16_t const templateLength);

// 	private:
// 	Color _Color[SINGLEEFFECT_COLORWITH];
// 	Color _ColorOld[SINGLEEFFECT_COLORWITH];
// 	void genImage(Color * color, EffectSM * effStat);
// };

// #define DUALEEFFECT_COLORWITH 2
// class DualEffect : public EffectProcessor {
// 	public:
// 	DualEffect(uint16_t const templateLength);

// 	private:
// 	Color _Color[DUALEEFFECT_COLORWITH];
// 	Color _ColorOld[DUALEEFFECT_COLORWITH];
// 	void genImage(Color * color, EffectSM * effStat);
// };

// class MultiEffect : public EffectProcessor {
// 	public:
// 	MultiEffect(uint8_t u8_Length);

// 	private:
// 	Color * _pColor;
// 	Color * _pColorOld;
// 	void genImage(Color * color, EffectSM * effStat);
// };
