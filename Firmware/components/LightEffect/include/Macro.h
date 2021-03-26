/**
 * @file Macro.h
 * @author Gustice
 * @brief Predefined effect macros 
 61
 * @date 2021-03-21
 * 
 * @copyright Copyright (c) 2021
 */

#pragma once

#include "Color.h"
#include "Basics.h"
#include "StateMachine.h"
#include <stdint.h>

namespace Effect {

/// Dark - Nothing to see here
extern Macro_t macDark[];
/// Smooth turn on and then remaining idle
extern Macro_t macStartIdle[];
/// Continuous idle 
extern Macro_t macIdle[];
/// Slow pulse starting from idle intensity. Appears like breathing 
extern Macro_t macStdPulse[];
/// Short pulses starting from idle intensity. Appears nervousely.
extern Macro_t macNervousPulse[];

/// Dark - Nothing to see here
extern const Sequence StdDark[];
/// Slow pulse starting from idle intensity. Appears like breathing 
extern const Sequence StdPulse[];


} // namespace Effect