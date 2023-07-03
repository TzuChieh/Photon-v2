#include "Actor/Material/MatteOpaque.h"
#include "Actor/Image/ConstantImage.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"

#include <utility>

namespace ph
{

MatteOpaque::MatteOpaque(const math::Vector3R& albedo) :

	SurfaceMaterial(),

	m_albedo(),
	m_sigmaDegrees()
{
	setAlbedo(albedo);
}

void MatteOpaque::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_albedo);

	std::shared_ptr<SurfaceOptics> optics;
	if(m_sigmaDegrees)
	{
		optics = std::make_shared<OrenNayar>(
			m_albedo->genColorTexture(ctx),
			m_sigmaDegrees->genRealTexture(ctx));
	}
	else
	{
		optics = std::make_shared<LambertianDiffuse>(
			m_albedo->genColorTexture(ctx));
	}

	behavior.setOptics(optics);
}

void MatteOpaque::setAlbedo(const math::Vector3R& albedo)
{
	getAlbedo()->setConstantColor(albedo, math::EColorSpace::Linear_sRGB);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	setAlbedo({r, g, b});
}

void MatteOpaque::setAlbedo(std::shared_ptr<Image> albedo)
{
	getAlbedo()->setImage(std::move(albedo));
}

UnifiedColorImage* MatteOpaque::getAlbedo()
{
	if(!m_albedo)
	{
		m_albedo = std::make_shared<UnifiedColorImage>();
	}

	return m_albedo.get();
}

}// end namespace ph
