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
class CookedMaterial;

class AGeometricLight : public ALight
{
public:
	/*!
	A geometric source would need to place a corresponding geometry in the scene. Override this
	method and return a geometry for that.
	*/
	virtual std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const = 0;

	/*! @brief Construct the surface emission part of the light source.
	@return A newly constructed emitter or `nullptr` on failure.
	The result is mutable so `cook()` can apply common emission settings.
	*/
	virtual SurfaceEmitter* buildSurfaceEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const = 0;

	virtual bool isVolumetricEmissionSupported() const = 0;

	/*!
	@return A material suitable for the light source if a physical entity will be present in the scene.
	*/
	virtual std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const;

	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setIsIntersectable(bool isIntersectable);
	bool isIntersectable() const;
	bool shouldEmitBackward() const;
	bool shouldFlipNg() const;
	void setIsDirectlyVisible(bool isDirectlyVisible);
	void setEmitBackward(bool shouldEmitBackward);
	void setShouldFlipNg(bool shouldFlipNg);

protected:
	/*!
	@return Create an emitter feature set from light settings.
	*/
	virtual EmitterFeatureSet getEmitterFeatureSet() const;

	bool m_isIntersectable;
	bool m_isDirectlyVisible;
	bool m_useBsdfSample;
	bool m_useDirectSample;
	bool m_useEmissionSample;

private:
	/*! @brief Get cooked geometry, baking the light's full transform if it contains scale.
	@return `nullptr` if no geometry is supplied.
	*/
	const CookedGeometry* getSanifiedGeometry(
		const CookingContext& ctx,
		std::shared_ptr<Geometry>* out_geometryResource = nullptr) const;

	/*! @brief Get cooked material when the light needs a physical surface.
	@return `nullptr` if non-intersectable or no material is supplied.
	*/
	const CookedMaterial* getSanifiedMaterial(
		const CookingContext& ctx,
		std::shared_ptr<Material>* out_materialResource = nullptr) const;

	bool m_shouldEmitBackward;
	bool m_shouldFlipNg;

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

		TSdlBool<OwnerType> intersectable("intersectable", &OwnerType::m_isIntersectable);
		intersectable.description(
			"Whether the light's material affects rays. When disabled, rays pass through without "
			"scattering or shadowing, while emission remains available to all enabled sampling "
			"techniques. This is a non-physical artistic control, independent of directly-visible.");
		intersectable.defaultTo(true);
		intersectable.optional();
		clazz.addField(intersectable);

		TSdlBool<OwnerType> directlyVisible("directly-visible", &OwnerType::m_isDirectlyVisible);
		directlyVisible.description(
			"Whether the light's emitted energy is visible before the camera ray interacts with "
			"any surface optics. Disabling this suppresses only zero-bounce emission; the light's "
			"material still interacts with rays normally. This is a non-physical artistic control.");
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

		TSdlBool<OwnerType> emitBackward("emit-backward", &OwnerType::m_shouldEmitBackward);
		emitBackward.description(
			"Emit opposite to the surface's shading normal without changing geometry orientation.");
		emitBackward.defaultTo(false);
		emitBackward.optional();
		clazz.addField(emitBackward);

		TSdlBool<OwnerType> shouldFlipNg("should-flip-ng", &OwnerType::m_shouldFlipNg);
		shouldFlipNg.description(
			"Flips the geometric normal (Ng) after transform and preserves explicit shading normals. "
			"To reverse light emission, use emit-backward.");
		shouldFlipNg.defaultTo(false);
		shouldFlipNg.optional();
		clazz.addField(shouldFlipNg);
	}
};

}// end namespace ph
