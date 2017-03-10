#pragma once

#include "ph_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API void phQueryRendererPercentageProgress(const PHuint64 engineId, PHfloat32* const out_percentage);
extern PH_API void phQueryRendererSampleFrequency(const PHuint64 engineId, PHfloat32* const out_frequency);

#ifdef __cplusplus
}
#endif