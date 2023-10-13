#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/sdl_interface.h"
//#include "Actor/SDLExtension/TSdlUnifiedColorImage.h"

#include <memory>
#include <string>

namespace ph
{

class Image;

class ModelSource : public LightSource
{
public:
	ModelSource();
	explicit ModelSource(const math::Vector3R& emittedRgbRadiance);
	explicit ModelSource(const Path& imagePath);
	explicit ModelSource(const std::shared_ptr<Image>& emittedRadiance);

	std::unique_ptr<Emitter> genEmitter(
		CookingContext& ctx, EmitterBuildingMaterial&& data) const override;

	std::shared_ptr<Geometry> genGeometry(CookingContext& ctx) const override;

	std::shared_ptr<Material> genMaterial(CookingContext& ctx) const override;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setBackFaceEmit(bool isBackFaceEmit);

private:
	//std::shared_ptr<UnifiedColorImage> m_emittedRadiance;
	std::shared_ptr<Geometry>          m_geometry;
	std::shared_ptr<Material>          m_material;
	bool                               m_isBackFaceEmit;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ModelSource>)
	{
		ClassType clazz("model");
		clazz.docName("Model Light Source");
		clazz.description(
			"A light source that emits energy from the surface of a geometry. A surface material model "
			"can also be given to describe its surface appearance.");
		clazz.baseOn<LightSource>();

		/*TSdlUnifiedColorImage<OwnerType> emittedRadiance("emitted-radiance", &OwnerType::m_emittedRadiance);
		emittedRadiance.description("An image that describes the emitted radiance across the surface.");
		emittedRadiance.defaultLinearSRGB(1.0_r);
		clazz.addField(emittedRadiance);*/

		TSdlReference<Geometry, OwnerType> geometry("geometry", &OwnerType::m_geometry);
		geometry.description("A geometry that defines the surface energy is going to emit from.");
		geometry.required();
		clazz.addField(geometry);

		TSdlReference<Material, OwnerType> material("material", &OwnerType::m_material);
		material.description("A material that describes this source's surface appearance.");
		material.optional();
		clazz.addField(material);

		TSdlBool<OwnerType> isBackFaceEmit("back-face-emit", &OwnerType::m_isBackFaceEmit);
		isBackFaceEmit.description("Whether the energy should emit from the back face of the geometry.");
		isBackFaceEmit.defaultTo(false);
		isBackFaceEmit.optional();
		clazz.addField(isBackFaceEmit);

		return clazz;
	}
};

}// end namespace ph
