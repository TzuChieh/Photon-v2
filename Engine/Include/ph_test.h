#pragma once

#include "ph_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API void printTestMessage();
extern PH_API void testRun();
extern PH_API void genTestHdrFrame(const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx);

#ifdef __cplusplus
}
#endif