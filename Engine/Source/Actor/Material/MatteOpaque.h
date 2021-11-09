#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Actor/SDLExtension/TSdlUnifiedColorImage.h"
#include "Actor/SDLExtension/TSdlUnifiedNumericImage.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial
{
public:
	inline MatteOpaque() = default;
	explicit MatteOpaque(const math::Vector3R& albedo);

	void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const override;

	/*!
	@param albedo Albedo in linear-sRGB.
	*/
	void setAlbedo(const math::Vector3R& albedo);

	/*! @brief Same as setAlbedo(const math::Vector3R&).
	*/
	void setAlbedo(const real r, const real g, const real b);

	void setAlbedo(std::shared_ptr<Image> albedo);

protected:
	UnifiedColorImage* getAlbedo();

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
