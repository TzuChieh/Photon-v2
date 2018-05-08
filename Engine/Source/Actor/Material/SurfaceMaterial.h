#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class SurfaceOptics;

class SurfaceMaterial : public Material, public TCommandInterface<SurfaceMaterial>
{
public:
	SurfaceMaterial();
	virtual ~SurfaceMaterial() override;

	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const = 0;
	virtual void genSurfaceBehavior(CookingContext& context, SurfaceBehavior* out_surfaceBehavior) const override;

// command interface
public:
	explicit SurfaceMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph