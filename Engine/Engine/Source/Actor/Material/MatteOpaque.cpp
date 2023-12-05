#include "Actor/Material/MatteOpaque.h"
#include "Actor/Image/ConstantImage.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"
#include "Actor/Image/SwizzledImage.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

void MatteOpaque::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	auto albedo = m_albedo;
	if(!albedo)
	{
		PH_DEFAULT_LOG(
			"No albedo specified. Default to 50% reflectance.");
		albedo = makeConstantAlbedo(math::Vector3R(0.5_r));
	}

	std::shared_ptr<SurfaceOptics> optics;
	if(m_sigmaDegrees)
	{
		auto sigmaDegrees = TSdl<SwizzledImage>::makeResource();
		sigmaDegrees->setInput(m_sigmaDegrees);
		sigmaDegrees->setSwizzleSubscripts("x");

		optics = std::make_shared<OrenNayar>(
			albedo->genColorTexture(ctx),
			sigmaDegrees->genRealTexture(ctx));
	}
	else
	{
		optics = std::make_shared<LambertianDiffuse>(
			albedo->genColorTexture(ctx));
	}

	behavior.setOptics(optics);
}

void MatteOpaque::setAlbedo(const math::Vector3R& albedo)
{
	setAlbedo(makeConstantAlbedo(albedo));
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	setAlbedo(math::Vector3R(r, g, b));
}

void MatteOpaque::setAlbedo(std::shared_ptr<Image> albedo)
{
	m_albedo = std::move(albedo);
}

std::shared_ptr<Image> MatteOpaque::makeConstantAlbedo(const math::Vector3R& albedo)
{
	auto imageAlbedo = TSdl<ConstantImage>::makeResource();
	imageAlbedo->setColor(albedo, math::EColorSpace::Linear_sRGB);
	return imageAlbedo;
}

}// end namespace ph
