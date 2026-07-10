#include "Engine/Actor/Material/ThinDielectricSurface.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/ThinDielectricShell.h"
#include "Engine/Actor/Image/SwizzledImage.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

void ThinDielectricSurface::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale = m_reflectionScale
		? m_reflectionScale->genColorTexture(ctx) : nullptr;
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale = m_transmissionScale
		? m_transmissionScale->genColorTexture(ctx) : nullptr;

	ThinDielectricShell* surfaceOptics = nullptr;
	if(!m_thickness && !m_sigmaT)
	{
		surfaceOptics = ctx.getResources().makeSurfaceOptics<ThinDielectricShell>(
			m_interfaceInfo.genFresnelEffect(ctx),
			reflectionScale,
			transmissionScale);
	}
	else
	{
		if(!m_thickness || !m_sigmaT)
		{
			PH_DEFAULT_LOG(Warning,
				"A thin dielectric surface will only have volumetric scattering simulated if both "
				"thickness ({}) and sigma_t ({}) are provided.",
				m_thickness ? "available" : "missing", m_sigmaT ? "available" : "missing");

			surfaceOptics = ctx.getResources().makeSurfaceOptics<ThinDielectricShell>(
				m_interfaceInfo.genFresnelEffect(ctx),
				reflectionScale,
				transmissionScale);
		}
		else
		{
			PH_ASSERT(m_thickness);
			PH_ASSERT(m_sigmaT);

			auto thickness = TSdl<SwizzledImage>::makeResource();
			thickness->setInput(m_thickness);
			thickness->setSwizzleSubscripts("x");

			surfaceOptics = ctx.getResources().makeSurfaceOptics<ThinDielectricShell>(
				m_interfaceInfo.genFresnelEffect(ctx),
				thickness->genRealTexture(ctx),
				m_sigmaT->genColorTexture(ctx),
				reflectionScale,
				transmissionScale);
		}
	}

	PH_ASSERT(surfaceOptics);
	out_material.surfaceOptics = surfaceOptics;
}

}// end namespace ph
