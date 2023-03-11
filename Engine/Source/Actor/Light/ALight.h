#pragma once

#include "Actor/PhysicalActor.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/LightSource/LightSource.h"
#include "Utility/TSpan.h"

#include <memory>
#include <vector>

namespace ph::math { class RigidTransform; }

namespace ph
{

class Emitter;
class Primitive;

class ALight : public PhysicalActor
{
public:
	/*virtual std::vector<const Emitter*> buildEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> primitives) const = 0;*/

	virtual bool isGeometric() const = 0;

	PreCookReport preCook(CookingContext& ctx) override;
	TransientVisualElement cook(CookingContext& ctx, const PreCookReport& report) override;

	const LightSource* getLightSource() const;
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

protected:
	std::shared_ptr<LightSource> m_lightSource;

	TransientVisualElement buildGeometricLight(
		CookingContext& ctx,
		const std::shared_ptr<Geometry>& srcGeometry,
		const std::shared_ptr<Material>& srcMaterial,
		const PreCookReport& report) const;

	/*!
	Tries to return a geometry suitable for emitter calculations (can be the 
	original one if it is already suitable). If the current actor has undesired 
	configurations, nullptr is returned.
	*/
	std::shared_ptr<Geometry> getSanifiedGeometry(
		const std::shared_ptr<Geometry>& srcGeometry,
		math::TDecomposedTransform<real>* out_remainingLocalToWorld) const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<ALight>)
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
