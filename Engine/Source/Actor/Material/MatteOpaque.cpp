#include "Actor/Material/MatteOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/PictureLoader.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	MatteOpaque(math::Vector3R(0.5_r))
{}

MatteOpaque::MatteOpaque(const math::Vector3R& linearSrgbAlbedo) :

	SurfaceMaterial(),

	m_albedo(),
	m_sigmaDegrees()
{
	setAlbedo(linearSrgbAlbedo);
}

void MatteOpaque::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_albedo);

	std::shared_ptr<SurfaceOptics> optics;
	if(m_sigmaDegrees)
	{
		optics = std::make_shared<OrenNayar>(
			m_albedo->genTextureSpectral(context),
			m_sigmaDegrees->genTextureReal(context));
	}
	else
	{
		optics = std::make_shared<LambertianDiffuse>(
			m_albedo->genTextureSpectral(context));
	}

	behavior.setOptics(optics);
}

void MatteOpaque::setAlbedo(const math::Vector3R& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	m_albedo = std::make_shared<ConstantImage>(std::vector<real>{r, g, b}, ConstantImage::EType::ECF_LINEAR_SRGB);
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Image>& albedo)
{
	m_albedo = albedo;
}

}// end namespace ph
