#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class World;

extern void loadTestScene(World* const out_world);
extern void loadCornellBox(World* const out_world, const real boxSize);
extern void load5bScene(World* const out_world);
extern void loadCbox3ObjScene(World* const out_world);

}// end namespace ph