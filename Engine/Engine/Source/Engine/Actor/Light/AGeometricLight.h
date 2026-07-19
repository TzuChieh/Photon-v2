#pragma once

#include "Engine/Actor/Light/ALight.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Utility/TSpan.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class Primitive;
class CookedGeometry;

class AGeometricLight : public ALight
{
public:
	/*!
	A geometric source would need to place a corresponding geometry in the scene. Override this
	method and return a geometry for that.
	*/
	virtual std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const = 0;

	/*!
	Generates the surface emission part of the light source.
	*/
	virtual const SurfaceEmitter* buildSurfaceEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const = 0;

	virtual bool isVolumetricEmissionSupported() const = 0;

	/*!
	@return A material suitable for the light source if a physical entity will be present in the scene.
	*/
	virtual std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const;

	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;
	void setShouldFlipNg(bool shouldFlipNg);
	bool shouldFlipNg() const;

	/*! @brief Get geometry cooked into a form suitable for emitter calculations.
	If @p srcLocalToWorld contains scale, its complete transform is baked into a dedicated cooked
	variant. Otherwise, the regular cooked geometry is used and the transform is left to primitive
	instancing.
	@return Cooked geometry, or nullptr if @p srcGeometry is empty.
	*/
	static const CookedGeometry* getSanifiedGeometry(
		const std::shared_ptr<Geometry>& srcGeometry,
		const TransformInfo& srcLocalToWorld,
		const CookingContext& ctx);

protected:
	/*!
	@return Create an emitter feature set from light settings.
	*/
	virtual EmitterFeatureSet getEmitterFeatureSet() const;

	bool m_isDirectlyVisible;
	bool m_useBsdfSample;
	bool m_useDirectSample;
	bool m_useEmissionSample;

private:
	bool m_shouldFlipNg = false;

public:
	PH_DEFINE_SDL_CLASS(AGeometricLight, clazz)
	{
		clazz.typeName("geometric-light");
		clazz.docName("Geometric Light Actor");
		clazz.description(
			"Energy emitters that come with a physical geometry. Please be aware that changing "
			"sampling techniques to non-default values may cause the rendered image to lose energy. "
			"For example, disabling BSDF sampling may cause some/all caustics to disappear "
			"on specular surfaces.");
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

		TSdlBool<OwnerType> shouldFlipNg("should-flip-ng", &OwnerType::m_shouldFlipNg);
		shouldFlipNg.description(
			"Flips only the geometric normal (Ng) after transform; the shading normal (Ns) is not flipped. "
			"Flipping Ng will also affect the side of emission.");
		shouldFlipNg.defaultTo(false);
		shouldFlipNg.optional();
		clazz.addField(shouldFlipNg);
	}
};

}// end namespace ph
