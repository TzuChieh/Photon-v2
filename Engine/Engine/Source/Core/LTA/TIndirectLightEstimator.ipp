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
	math::Spectrum* const  out_Lo,
	const std::size_t      rrBeginPathLength,
	const math::Spectrum&  initialPathWeight) const
{
	PH_ASSERT_GE(pathLength, 1);

	const SurfaceTracer surfaceTracer{m_scene};
	const TDirectLightEstimator<POLICY> directLight{m_scene};
	const BsdfQueryContext bsdfCtx{POLICY};

	SurfaceHit currentHit = X;
	math::Spectrum pathThroughput = initialPathWeight;
	real rrScale = 1.0_r;

	std::size_t currentPathLength = 0;
	while(currentPathLength < pathLength)// can extend by 1?
	{
		if(currentPathLength >= rrBeginPathLength)
		{
			real rrSurvivalProb;
			if(rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				pathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				return false;
			}
		}

		const math::Vector3R V = currentHit.getIncidentRay().getDir().mul(-1);

		BsdfSampleQuery bsdfSample{bsdfCtx};
		bsdfSample.inputs.set(currentHit, V);

		// Sample direct lighting
		if(currentPathLength + 1 == pathLength)
		{
			math::Spectrum Lo;
			if(directLight.bsdfSamplePathWithNee(bsdfSample, sampleFlow, &Lo))
			{
				if(out_Lo) { *out_Lo = pathThroughput * Lo; }
				return true;
			}
		}
		// Extend the path
		else
		{
			SurfaceHit nextHit;
			if(!surfaceTracer.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextHit) ||
			   !bsdfSample.outputs.isMeasurable())
			{
				return false;
			}

			currentHit = nextHit;
			pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

			// Prevent premature termination of the path due to solid angle compression/expansion
			rrScale /= bsdfSample.outputs.getRelativeIor2();
		}

		++currentPathLength;
	}

	return false;
}

template<ESidednessPolicy POLICY>
inline bool TIndirectLightEstimator<POLICY>::bsdfSamplePathWithNee(
	const SurfaceHit&      X,
	SampleFlow&            sampleFlow,
	const std::size_t      minPathLength,
	const std::size_t      maxPathLength,
	const RussianRoulette& rr,
	math::Spectrum* const  out_Lo,
	const std::size_t      rrBeginPathLength,
	const math::Spectrum&  initialPathWeight) const
{
	PH_ASSERT_GE(minPathLength, 1);
	PH_ASSERT_LE(minPathLength, maxPathLength);

	const SurfaceTracer surfaceTracer{m_scene};
	const TDirectLightEstimator<POLICY> directLight{m_scene};
	const BsdfQueryContext bsdfCtx{POLICY};

	SurfaceHit currentHit = X;
	math::Spectrum pathThroughput = initialPathWeight;
	math::Spectrum accuLo(0);
	real rrScale = 1.0_r;

	std::size_t currentPathLength = 0;
	while(currentPathLength < maxPathLength)// can extend by 1?
	{
		if(currentPathLength >= rrBeginPathLength)
		{
			real rrSurvivalProb;
			if(rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				pathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				break;
			}
		}

		const math::Vector3R V = currentHit.getIncidentRay().getDir().mul(-1);

		BsdfSampleQuery bsdfSample{bsdfCtx};
		bsdfSample.inputs.set(currentHit, V);

		// Account for energy from the specified path length range
		PH_ASSERT_LE(currentPathLength + 1, maxPathLength);
		if(currentPathLength + 1 >= minPathLength)
		{
			math::Spectrum Lo;
			std::optional<SurfaceHit> nextHit;
			if(!directLight.bsdfSamplePathWithNee(bsdfSample, sampleFlow, &Lo, &nextHit) ||
			   !bsdfSample.outputs.isMeasurable() ||
			   !nextHit)
			{
				break;
			}

			accuLo += pathThroughput * Lo;
			currentHit = *nextHit;
		}
		// Extend the path
		else
		{
			SurfaceHit nextHit;
			if(!surfaceTracer.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextHit) ||
			   !bsdfSample.outputs.isMeasurable())
			{
				break;
			}

			currentHit = nextHit;
		}

		pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

		// Prevent premature termination of the path due to solid angle compression/expansion
		rrScale /= bsdfSample.outputs.getRelativeIor2();

		++currentPathLength;
	}

	if(out_Lo) { *out_Lo = accuLo; }

	return true;
}

}// end namespace ph::lta
