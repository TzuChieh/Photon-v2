#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "Actor/Image/UnifiedColorImage.h"
#include "Actor/Image/UnifiedNumericImage.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial
{
public:
	MatteOpaque();
	explicit MatteOpaque(const math::Vector3R& linearSrgbAlbedo);

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;

	void setAlbedo(const math::Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Image>& albedo);

private:
	std::shared_ptr<UnifiedColorImage>   m_albedo;
	std::shared_ptr<UnifiedNumericImage> m_sigmaDegrees;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<MatteOpaque>)
	{
		ClassType clazz("matte-opaque");
		clazz.docName("Matte Opaque Material");
		clazz.description("A material model for surfaces with matte look, such as chalk and moon.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlUnifiedColorImage<OwnerType> albedo("albedo", &OwnerType::m_albedo);
		albedo.description("An image or constant color that will be used for describing albedo.");
		albedo.defaultLinearSRGB(0.5_r);
		clazz.addField(albedo);

		TSdlUnifiedNumericImage<OwnerType> sigmaDegrees("sigma-degrees", &OwnerType::m_sigmaDegrees);
		sigmaDegrees.description("Roughness in standard deviation of surface orientation (unit: degrees).");
		sigmaDegrees.noDefault();
		sigmaDegrees.optional();
		clazz.addField(sigmaDegrees);

		return clazz;
	}
};

}// end namespace ph
