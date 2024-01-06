#pragma once

#include "Core/LTA/TIndirectLightEstimator.h"
#include "Core/LTA/TDirectLightEstimator.h"
#include "Core/LTA/RussianRoulette.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceHit.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

namespace ph::lta
{

template<ESidednessPolicy POLICY>
inline TIndirectLightEstimator<POLICY>::TIndirectLightEstimator(const Scene* const scene)
	: m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessPolicy POLICY>
inline bool TIndirectLightEstimator<POLICY>::bsdfSamplePathWithNee(
	const SurfaceHit&      X,
	SampleFlow&            sampleFlow,
	const std::size_t      pathLength,
	const RussianRoulette& rr,
	math::Spectrum* const  out_Lo) const
{
	PH_ASSERT_GE(pathLength, 1);

	const SurfaceTracer surfaceTracer{m_scene};
	const TDirectLightEstimator<POLICY> directLight{m_scene};
	const BsdfQueryContext bsdfCtx{POLICY};

	SurfaceHit surfaceHit = X;
	math::Spectrum accuLiWeight(1);

	auto remainingPathLength = pathLength;
	while(remainingPathLength > 0)
	{
		const math::Vector3R V = surfaceHit.getIncidentRay().getDirection().mul(-1);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		// Sample direct lighting
		if(remainingPathLength == 1)
		{
			math::Spectrum Lo;
			if(directLight.bsdfSampleOutgoingWithNee(
				surfaceHit,
				sampleFlow,
				&Lo))
			{
				if(out_Lo) { *out_Lo = accuLiWeight * Lo; }
				return true;
			}
		}
		// Extend the path
		else
		{
			BsdfSampleQuery bsdfSample{bsdfCtx};
			bsdfSample.inputs.set(surfaceHit, V);

			SurfaceHit nextSurfaceHit;
			if(!surfaceTracer.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextSurfaceHit))
			{
				return false;
			}

			surfaceHit = nextSurfaceHit;
			accuLiWeight *= bsdfSample.outputs.pdfAppliedBsdf * N.absDot(bsdfSample.outputs.L);
		}

		math::Spectrum weightedAccuLiWeight;
		if(rr.surviveOnLuminance(accuLiWeight, sampleFlow, &weightedAccuLiWeight))
		{
			accuLiWeight = weightedAccuLiWeight;
		}
		else
		{
			return false;
		}

		--remainingPathLength;
	}

	return false;
}

}// end namespace ph::lta