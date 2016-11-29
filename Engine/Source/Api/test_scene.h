#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class World;

extern void loadTestScene(World* const out_world);
extern void loadCornellBox(World* const out_world, const float32 boxSize);
extern void load5bScene(World* const out_world);

}// end namespace ph