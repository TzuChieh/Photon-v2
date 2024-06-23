#pragma once

#include "Actor/Light/AGeometricLight.h"
#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class AModelLight : public AGeometricLight
{
public:
	std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const override;
	std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const override;

	const Emitter* buildEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const override;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setBackFaceEmit(bool isBackFaceEmit);

private:
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
	std::shared_ptr<Image> m_emittedEnergy;
	bool m_isBackFaceEmit;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AModelLight>)
	{
		ClassType clazz("model-light");
		clazz.docName("Model Light Actor");
		clazz.description(
			"A light source that emits energy from the surface of a geometry. A surface material model "
			"can also be given to describe its surface appearance.");
		clazz.baseOn<AGeometricLight>();

		TSdlReference<Geometry, OwnerType> geometry("geometry", &OwnerType::m_geometry);
		geometry.description("A geometry that defines the surface energy is going to emit from.");
		geometry.required();
		clazz.addField(geometry);

		TSdlReference<Material, OwnerType> material("material", &OwnerType::m_material);
		material.description("A material that describes this source's surface appearance.");
		material.optional();
		clazz.addField(material);

		TSdlReference<Image, OwnerType> emittedEnergy("emitted-energy", &OwnerType::m_emittedEnergy);
		emittedEnergy.description(
			"An image that describes the emitted energy (e.g., radiance) across the surface.");
		emittedEnergy.optional();
		clazz.addField(emittedEnergy);

		TSdlBool<OwnerType> isBackFaceEmit("back-face-emit", &OwnerType::m_isBackFaceEmit);
		isBackFaceEmit.description("Whether the energy should emit from the back face of the geometry.");
		isBackFaceEmit.defaultTo(false);
		isBackFaceEmit.optional();
		clazz.addField(isBackFaceEmit);

		return clazz;
	}
};

}// end namespace ph
