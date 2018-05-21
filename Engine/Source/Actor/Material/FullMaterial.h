#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class InputPacket;
class CookingContext;
class SurfaceMaterial;
class VolumeMaterial;

// TODO: volume material

class FullMaterial final : public Material, public TCommandInterface<FullMaterial>
{
public:
	FullMaterial();
	FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial);
	virtual ~FullMaterial() override;

	void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_surfaceMaterial;
	std::shared_ptr<VolumeMaterial>  m_interiorMaterial;
	std::shared_ptr<VolumeMaterial>  m_exteriorMaterial;

// command interface
public:
	explicit FullMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph