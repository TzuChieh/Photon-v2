#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial
{
public:
	void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const override;

	/*!
	@param albedo Albedo in linear-sRGB.
	*/
	void setAlbedo(const math::Vector3R& albedo);

	/*! @brief Same as setAlbedo(const math::Vector3R&).
	*/
	void setAlbedo(const real r, const real g, const real b);

	void setAlbedo(std::shared_ptr<Image> albedo);

private:
	/*!
	@param albedo Albedo in linear-sRGB.
	*/
	static std::shared_ptr<Image> makeConstantAlbedo(const math::Vector3R& albedo);

	std::shared_ptr<Image> m_albedo;
	std::shared_ptr<Image> m_sigmaDegrees;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<MatteOpaque>)
	{
		ClassType clazz("matte-opaque");
		clazz.docName("Matte Opaque Material");
		clazz.description("A material model for surfaces with matte look, such as chalk and moon.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlReference<Image, OwnerType> albedo("albedo", &OwnerType::m_albedo);
		albedo.description("An image or constant color that will be used for describing albedo.");
		clazz.addField(albedo);

		TSdlReference<Image, OwnerType> sigmaDegrees("sigma-degrees", &OwnerType::m_sigmaDegrees);
		sigmaDegrees.description("Roughness in standard deviation of surface orientation (unit: degrees).");
		sigmaDegrees.optional();
		clazz.addField(sigmaDegrees);

		return clazz;
	}
};

}// end namespace ph
