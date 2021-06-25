#pragma once

#include "Math/Transform/StaticAffineTransform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/PhysicalActor.h"
#include "Math/Transform/StaticRigidTransform.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class LightSource;

namespace math
{
	class Transform;
}

class ALight : public PhysicalActor
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);

	CookedUnit cook(CookingContext& context) override;

	const LightSource* getLightSource() const;
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

	ALight& operator = (ALight rhs);

	friend void swap(ALight& first, ALight& second);

private:
	std::shared_ptr<LightSource> m_lightSource;

	CookedUnit buildGeometricLight(
		CookingContext&           context, 
		std::shared_ptr<Geometry> geometry,
		std::shared_ptr<Material> material) const;

	// Tries to return a geometry suitable for emitter calculations (can be the 
	// original one if it is already suitable). If the current actor has undesired 
	// configurations, nullptr is returned.
	std::shared_ptr<Geometry> getSanifiedGeometry(
		CookingContext&                        context,
		const std::shared_ptr<Geometry>&       geometry,
		std::unique_ptr<math::RigidTransform>* out_baseLW,
		std::unique_ptr<math::RigidTransform>* out_baseWL) const;
};

}// end namespace ph
