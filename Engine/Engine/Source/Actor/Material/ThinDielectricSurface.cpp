#include "Actor/Material/ThinDielectricSurface.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/ThinDielectricShell.h"
#include "Actor/Image/SwizzledImage.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

void ThinDielectricSurface::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	std::shared_ptr<TTexture<math::Spectrum>> reflectionScale = m_reflectionScale
		? m_reflectionScale->genColorTexture(ctx) : nullptr;
	std::shared_ptr<TTexture<math::Spectrum>> transmissionScale = m_transmissionScale
		? m_transmissionScale->genColorTexture(ctx) : nullptr;

	std::unique_ptr<ThinDielectricShell> surfaceOptics;
	if(!m_thickness && !m_sigmaT)
	{
		surfaceOptics = std::make_unique<ThinDielectricShell>(
			m_interfaceInfo.genFresnelEffect(),
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

			surfaceOptics = std::make_unique<ThinDielectricShell>(
				m_interfaceInfo.genFresnelEffect(),
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

			surfaceOptics = std::make_unique<ThinDielectricShell>(
				m_interfaceInfo.genFresnelEffect(),
				thickness->genRealTexture(ctx),
				m_sigmaT->genColorTexture(ctx),
				reflectionScale,
				transmissionScale);
		}
	}

	PH_ASSERT(surfaceOptics);
	behavior.setOptics(std::move(surfaceOptics));
}

}// end namespace ph
