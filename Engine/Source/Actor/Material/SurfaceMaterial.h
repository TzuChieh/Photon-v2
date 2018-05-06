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

	virtual void genSurfaceBehavior(CookingContext& context, SurfaceBehavior* out_surfaceBehavior) const override;

private:
	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const = 0;

// command interface
public:
	SurfaceMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph