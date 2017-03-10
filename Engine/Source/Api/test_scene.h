#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class VisualWorld;

extern void loadTestScene(VisualWorld* const out_world);
extern void loadCornellBox(VisualWorld* const out_world, const real boxSize);
extern void load5bScene(VisualWorld* const out_world);
extern void loadCbox3ObjScene(VisualWorld* const out_world);

}// end namespace ph