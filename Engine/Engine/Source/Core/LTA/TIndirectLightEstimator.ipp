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

	SurfaceHit surfaceHit = X;
	math::Spectrum accuLiWeight = initialPathWeight;

	std::size_t currentPathLength = 0;
	while(currentPathLength < pathLength)
	{
		if(currentPathLength >= rrBeginPathLength)
		{
			math::Spectrum weightedAccuLiWeight;
			if(rr.surviveOnLuminance(accuLiWeight, sampleFlow, &weightedAccuLiWeight))
			{
				accuLiWeight = weightedAccuLiWeight;
			}
			else
			{
				return false;
			}
		}

		const math::Vector3R V = surfaceHit.getIncidentRay().getDirection().mul(-1);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		// Sample direct lighting
		if(currentPathLength + 1 == pathLength)
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
			accuLiWeight *= bsdfSample.outputs.getPdfAppliedBsdf() * N.absDot(bsdfSample.outputs.getL());
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

	const TDirectLightEstimator<POLICY> directLight{m_scene};

	SurfaceHit surfaceHit = X;
	math::Spectrum accuPathWeight = initialPathWeight;
	math::Spectrum accuLo(0);

	std::size_t currentPathLength = 0;
	while(currentPathLength + 1 <= maxPathLength)// can extend by 1?
	{
		if(currentPathLength >= rrBeginPathLength)
		{
			math::Spectrum weightedAccuPathWeight;
			if(rr.surviveOnLuminance(accuPathWeight, sampleFlow, &weightedAccuPathWeight))
			{
				accuPathWeight = weightedAccuPathWeight;
			}
			else
			{
				break;
			}
		}

		const math::Vector3R V = surfaceHit.getIncidentRay().getDirection().mul(-1);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		math::Spectrum Lo;
		math::Vector3R L;
		math::Spectrum pdfAppliedBsdf;
		SurfaceHit nextSurfaceHit;
		if(directLight.bsdfSampleOutgoingWithNee(
			surfaceHit,
			sampleFlow,
			&Lo,
			&L,
			&pdfAppliedBsdf,
			&nextSurfaceHit))
		{
			++currentPathLength;

			// Only account energy from the specified path length range
			PH_ASSERT_LE(currentPathLength, maxPathLength);
			if(minPathLength <= currentPathLength)
			{
				accuLo += accuPathWeight * Lo;
			}

			accuPathWeight *= pdfAppliedBsdf * N.absDot(L);
			surfaceHit = nextSurfaceHit;
		}
		else
		{
			break;
		}
	}

	if(out_Lo) { *out_Lo = accuLo; }

	return true;
}

}// end namespace ph::lta
