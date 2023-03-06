#pragma once

#include "Actor/Light/ALight.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/LightSource/LightSource.h"

#include <memory>
#include <vector>

namespace ph::math { class RigidTransform; }

namespace ph
{

class AAreaLight : public ALight
{
public:
	PreCookReport preCook(CookingContext& ctx) override;
	TransientVisualElement cook(CookingContext& ctx, const PreCookReport& report) override;

	const LightSource* getLightSource() const;
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

private:
	std::shared_ptr<LightSource> m_lightSource;

	TransientVisualElement buildGeometricLight(
		CookingContext& ctx,
		std::shared_ptr<Geometry> geometry,
		std::shared_ptr<Material> material,
		const PreCookReport& report) const;

	// Tries to return a geometry suitable for emitter calculations (can be the 
	// original one if it is already suitable). If the current actor has undesired 
	// configurations, nullptr is returned.
	std::shared_ptr<Geometry> getSanifiedGeometry(
		CookingContext& ctx,
		const std::shared_ptr<Geometry>& srcGeometry,
		math::TDecomposedTransform<real>* out_remainingLocalToWorld) const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AAreaLight>)
	{
		ClassType clazz("light");
		clazz.docName("Light Actor");
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
