#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class SurfaceOptics;
class SurfaceBehavior;

class SurfaceMaterial : public Material, public TCommandInterface<SurfaceMaterial>
{
public:
	SurfaceMaterial();

	virtual void genSurface(CookingContext& context, SurfaceBehavior& behavior) const = 0;

	void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const override;

// command interface
public:
	explicit SurfaceMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph