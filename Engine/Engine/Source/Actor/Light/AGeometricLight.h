#pragma once

#include "Actor/Light/ALight.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/Geometry.h"
#include "Core/Emitter/Emitter.h"
#include "Utility/TSpan.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class Primitive;

class AGeometricLight : public ALight
{
public:
	/*!
	A geometric source would need to place a corresponding geometry in the scene. Override this
	method and return a geometry for that.
	*/
	virtual std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const = 0;

	/*!
	Generates the core emission part of the light source.
	*/
	virtual const Emitter* buildEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const = 0;

	/*!
	Generate a suitable material for the light source if physical entity will be present in the scene.
	The default implementation generates a pure diffuse material with moderate albedo.
	*/
	virtual std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const;

	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	/*!
	Tries to return a geometry suitable for emitter calculations (can be the 
	original one if it is already suitable). If the current actor has undesired 
	configurations, nullptr is returned.
	*/
	static std::shared_ptr<Geometry> getSanifiedGeometry(
		const std::shared_ptr<Geometry>& srcGeometry,
		const TransformInfo& srcLocalToWorld,
		math::TDecomposedTransform<real>* out_remainingLocalToWorld = nullptr);

protected:
	/*!
	@return Create an emitter feature set from light settings.
	*/
	virtual EmitterFeatureSet getEmitterFeatureSet() const;

	bool m_isDirectlyVisible;
	bool m_useBsdfSample;
	bool m_useDirectSample;
	bool m_useEmissionSample;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AGeometricLight>)
	{
		ClassType clazz("geometric-light");
		clazz.docName("Geometric Light Actor");
		clazz.description(
			"Energy emitters that come with a physical geometry. Please be aware that changing "
			"sampling techniques to non-default values may cause the rendered image to lose energy. "
			"For example, disabling BSDF sampling may cause some/all caustics to disappear on "
			"specular surfaces.");
		clazz.baseOn<ALight>();

		TSdlBool<OwnerType> directlyVisible("directly-visible", &OwnerType::m_isDirectlyVisible);
		directlyVisible.description(
			"Whether the light is directly visible. For example, you can see a bright sphere "
			"for a directly visible spherical area light.");
		directlyVisible.defaultTo(true);
		directlyVisible.optional();
		clazz.addField(directlyVisible);

		TSdlBool<OwnerType> bsdfSample("bsdf-sample", &OwnerType::m_useBsdfSample);
		bsdfSample.description(
			"Whether to use BSDF sampling technique for rendering the light, i.e., choosing a "
			"direction based on BSDF and relying on randomly hitting a light.");
		bsdfSample.defaultTo(true);
		bsdfSample.optional();
		clazz.addField(bsdfSample);

		TSdlBool<OwnerType> directSample("direct-sample", &OwnerType::m_useDirectSample);
		directSample.description(
			"Whether to use direct sampling technique for rendering the light, i.e., directly "
			"establish a connection from a light to the illuminated location.");
		directSample.defaultTo(true);
		directSample.optional();
		clazz.addField(directSample);

		TSdlBool<OwnerType> emissionSample("emission-sample", &OwnerType::m_useEmissionSample);
		emissionSample.description(
			"Whether to use emission sampling technique for rendering the light, i.e., start "
			"rendering the light from the light source itself.");
		emissionSample.defaultTo(true);
		emissionSample.optional();
		clazz.addField(emissionSample);

		return clazz;
	}
};

}// end namespace ph
