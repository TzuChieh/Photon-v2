#pragma once

#include "ph_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API void phCreateHdrFrame(PHuint64* out_frameId, const PHuint32 widthPx, const PHuint32 heightPx);
extern PH_API void phDeleteHdrFrame(const PHuint64 frameId);

#ifdef __cplusplus
}
#endif