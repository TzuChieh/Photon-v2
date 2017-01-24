#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"

namespace ph
{

class SurfaceBehavior;
class BSDFcos;
class InputPacket;

class Material
{
public:
	Material();
	Material(const InputPacket& packet);
	virtual ~Material() = 0;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const = 0;
};

}// end namespace ph