#pragma once

#include "Actor/Material/Material.h"

#include <memory>

namespace ph
{

class SurfaceMaterial;
class VolumeMaterial;

// TODO: volume material

class FullMaterial final : public Material
{
public:
	FullMaterial();
	explicit FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial);

	void genBehaviors(ActorCookingContext& ctx, PrimitiveMetadata& metadata) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_surfaceMaterial;
	std::shared_ptr<VolumeMaterial>  m_interiorMaterial;
	std::shared_ptr<VolumeMaterial>  m_exteriorMaterial;
};

}// end namespace ph
