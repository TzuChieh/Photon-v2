#include "Engine/Actor/Material/MatteOpaque.h"
#include "Engine/Actor/Image/ConstantImage.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LambertianReflector.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"
#include "Engine/Actor/Image/SwizzledImage.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

void MatteOpaque::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	auto albedo = m_albedo;
	if(!albedo)
	{
		PH_DEFAULT_LOG(Note,
			"No albedo specified. Default to 50% reflectance.");
		albedo = makeConstantAlbedo(math::Vector3R(0.5_r));
	}

	const SurfaceOptics* optics = nullptr;
	if(m_sigmaDegrees)
	{
		auto sigmaDegrees = TSdl<SwizzledImage>::makeResource();
		sigmaDegrees->setInput(m_sigmaDegrees);
		sigmaDegrees->setSwizzleSubscripts("x");

		optics = ctx.getResources().makeSurfaceOptics<OrenNayar>(
			albedo->genColorTexture(ctx),
			sigmaDegrees->genRealTexture(ctx));
	}
	else
	{
		optics = ctx.getResources().makeSurfaceOptics<LambertianReflector>(
			albedo->genColorTexture(ctx));
	}

	out_material.surfaceOptics = optics;
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
