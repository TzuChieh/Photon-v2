#pragma once

#include "Actor/PhysicalActor.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/LightSource/LightSource.h"

#include <memory>
#include <vector>

namespace ph::math { class RigidTransform; }

namespace ph
{

class ALight : public PhysicalActor
{
public:
	ALight();
	explicit ALight(const std::shared_ptr<LightSource>& lightSource);
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

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<ALight>)
	{
		ClassType clazz("light");
		clazz.description("An actor that represents a light in the scene.");
		clazz.baseOn<PhysicalActor>();

		TSdlReference<LightSource, OwnerType> source("source", &OwnerType::m_lightSource);
		source.description("The source of the energy.");
		source.required();
		clazz.addField(source);

		return clazz;
	}
};

}// end namespace ph
