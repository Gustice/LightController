
#pragma once

#include "Color.h"
#include <stdint.h>

enum e_EffectPart {
    Light_Idle = 0,
    Light_Wave,
    Light_InvWave,
    Light_Flicker,
    Light_Sparkle,
    Light_Freeze,
    Light_Blank,

    Light_States,
};

typedef const struct EffectMacroElement_def {
    e_EffectPart   state;          // SubEffect
    uint8_t const *pu8_wave;       // waveform reference
    uint8_t        u8_FSintensity; // Full-Scale-intensity
    uint8_t        u8_duration;    // Effect duration
    const Color_t *color;          // SubEffect color
    int8_t         u8_repeats;     // Num of Repeats
    int8_t         next;           // Next SubEffect
} EffMacro_type;

class EffMacro {
  public:
    EffMacro(e_EffectPart state, uint8_t const *pu8_wave, uint8_t u8_FSintensity, uint8_t u8_duration,
             Color const *color, int8_t u8_repeats, int8_t next);

    e_EffectPart   _state;          // SubEffect
    uint8_t const *_pu8_wave;       // waveform reference
    uint8_t        _u8_FSintensity; // Full-Scale-intensity
    uint8_t        _u8_duration;    // Effect duration
    Color const *  _color;          // SubEffect color
    int8_t         _u8_repeats;     // Num of Repeats
    int8_t         _next;           // Next SubEffect
};
class DualEffMacro : EffMacro {
  public:
    DualEffMacro(e_EffectPart state, e_EffectPart stateI, uint8_t const *pu8_wave, uint8_t const *pu8_waveI,
                 uint8_t u8_FSintensity, uint8_t u8_duration, Color const *color, Color const *colorI,
                 int8_t u8_repeats,
                 int8_t next); // : EffMacro(state, pu8_wave, u8_FSintensity, u8_duration, color, u8_repeats, next);

    e_EffectPart   _stateI;    // SubEffect
    uint8_t const *_pu8_waveI; // waveform reference
    Color const *  _colorI;    // SubEffect color
};





#define COUNT_EFFECT_ELEMENT(effect) (sizeof(effect)/sizeof(EffMacro_type))

extern EffMacro_type eff_Dark[];
extern EffMacro_type eff_StartIdle[];
extern EffMacro_type eff_Idle[];
extern EffMacro_type eff_StdPulse[];
extern EffMacro_type eff_NervousPulse[];
extern EffMacro_type eff_StdWipe[];
extern EffMacro_type eff_AsymPulse[];
extern EffMacro_type eff_ShiftAsymPulse[];

typedef struct EffectList_def {
	const EffMacro_type * Macro;		// Macro reference
	const uint8_t u8_parts;				// Macro-entries
}EffectList_type;


typedef struct EffektSequenceElement_def {
	const EffMacro_type * element;	// VisualisierungsElement
	uint8_t u8_content;				// Anzahl der eingetragenen Teilschritte
	uint8_t u8_repeats;				// Wiederholungen
}EffSequence_type;
#define COUNT_EFFECT_MAKRO(effect) (sizeof(effect)/sizeof(EffSequence_type))
extern const EffSequence_type em_Idle[];
