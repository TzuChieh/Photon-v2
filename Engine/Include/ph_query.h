#pragma once

#include "ph_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API void phQueryRendererPercentageProgress(const PHuint64 rendererId, PHfloat32* const out_percentage);
extern PH_API void phQueryRendererIntermediateFilm(const PHuint64 rendererId, const PHuint64 filmId);

#ifdef __cplusplus
}
#endif