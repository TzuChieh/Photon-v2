#include "Engine/Actor/Material/MatteOpaque.h"
#include "Engine/Actor/SDLExtension/sdl_spectrum_io.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LambertianReflector.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TOrenNayar.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <utility>

namespace ph
{

void MatteOpaque::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	const SurfaceOptics* optics = nullptr;
	if(m_sigmaMap)
	{
		using Sigma = TTexturedSurfaceProperty<real>;

		if(m_albedoMap)
		{
			using Albedo = TTexturedSurfaceProperty<math::Spectrum, math::EColorUsage::ECF>;

			optics = ctx.getResources().makeSurfaceOptics<TOrenNayar<Albedo, Sigma>>(
				Albedo(m_albedoMap->genColorTexture(ctx)),
				Sigma(m_sigmaMap->genRealTexture(ctx)));
		}
		else
		{
			using Albedo = TConstantSurfaceProperty<math::Spectrum>;

			optics = ctx.getResources().makeSurfaceOptics<TOrenNayar<Albedo, Sigma>>(
				Albedo(m_albedo),
				Sigma(m_sigmaMap->genRealTexture(ctx)));
		}
	}
	else if(m_sigma != 0.0_r)
	{
		using Sigma = TConstantSurfaceProperty<real>;

		if(m_albedoMap)
		{
			using Albedo = TTexturedSurfaceProperty<math::Spectrum, math::EColorUsage::ECF>;

			optics = ctx.getResources().makeSurfaceOptics<TOrenNayar<Albedo, Sigma>>(
				Albedo(m_albedoMap->genColorTexture(ctx)),
				Sigma(m_sigma));
		}
		else
		{
			using Albedo = TConstantSurfaceProperty<math::Spectrum>;

			optics = ctx.getResources().makeSurfaceOptics<TOrenNayar<Albedo, Sigma>>(
				Albedo(m_albedo),
				Sigma(m_sigma));
		}
	}
	else
	{
		const std::shared_ptr<TTexture<math::Spectrum>> albedoTexture = m_albedoMap
			? m_albedoMap->genColorTexture(ctx)
			: std::make_shared<TConstantTexture<math::Spectrum>>(m_albedo);

		optics = ctx.getResources().makeSurfaceOptics<LambertianReflector>(
			albedoTexture);
	}

	out_material.surfaceOptics = optics;
}

void MatteOpaque::setAlbedo(const math::Vector3R& albedo)
{
	m_albedo = sdl::tristimulus_to_spectrum(
		math::TVector3<math::ColorValue>(albedo),
		math::EColorSpace::Linear_sRGB,
		math::EColorUsage::ECF);
}

void MatteOpaque::setAlbedo(const real r, const real g, const real b)
{
	setAlbedo(math::Vector3R(r, g, b));
}

void MatteOpaque::setAlbedoMap(std::shared_ptr<Image> albedoMap)
{
	m_albedoMap = std::move(albedoMap);
}

void MatteOpaque::setSigma(const real sigma)
{
	PH_ASSERT_GE(sigma, 0);
	m_sigma = sigma;
}

void MatteOpaque::setSigmaMap(std::shared_ptr<Image> sigmaMap)
{
	m_sigmaMap = std::move(sigmaMap);
}

}// end namespace ph
