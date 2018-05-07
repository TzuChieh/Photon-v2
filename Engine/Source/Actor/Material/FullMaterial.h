#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class SurfaceBehavior;
class InputPacket;
class CookingContext;
class SurfaceMaterial;

// TODO: volume material

class FullMaterial final : public Material, public TCommandInterface<FullMaterial>
{
public:
	FullMaterial();
	FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial);
	virtual ~FullMaterial() override;

	virtual void genSurfaceBehavior(CookingContext& context, SurfaceBehavior* out_surfaceBehavior) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_surfaceMaterial;

// command interface
public:
	FullMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph