#pragma once

#include "Actor/Light/ALight.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/Geometry.h"
#include "Utility/TSpan.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class Emitter;
class Primitive;

class AGeometricLight : public ALight
{
public:
	virtual std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const = 0;

	virtual const Emitter* buildEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const = 0;

	virtual std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const;

	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) override;

	/*!
	Tries to return a geometry suitable for emitter calculations (can be the 
	original one if it is already suitable). If the current actor has undesired 
	configurations, nullptr is returned.
	*/
	static std::shared_ptr<Geometry> getSanifiedGeometry(
		const std::shared_ptr<Geometry>& srcGeometry,
		const TransformInfo& srcLocalToWorld,
		math::TDecomposedTransform<real>* out_remainingLocalToWorld = nullptr);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AGeometricLight>)
	{
		ClassType clazz("geometric-light");
		clazz.docName("Geometric Light Actor");
		clazz.description("Energy emitters that come with a physical geometry.");
		clazz.baseOn<ALight>();

		return clazz;
	}
};

}// end namespace ph
